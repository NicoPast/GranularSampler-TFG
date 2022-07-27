#pragma once

template<typename T>
struct Fifo
{
    void prepare(int numChannels, int numSamples)
    {
        static_assert(std::is_same_v<T, juce::AudioBuffer<float>>,
            "prepare(numChannels, numSamples) should only be used when the Fifo is holding juce::AudioBuffer<float>");
        for (auto& buffer : buffers) {
            buffer.setSize(numChannels,
                numSamples,
                false,          //clear everything?
                true,           //including the extra space?
                true);          //avoid reallocating if you can?
            buffer.clear();
        }
    }

    void prepare(size_t numElements)
    {
        static_assert(std::is_same_v<T, std::vector<float>>,
            "prepare(numElements) should only be used when the Fifo is holding std::vector<float>");
        for (auto& buffer : buffers)
        {
            buffer.clear();
            buffer.resize(numElements, 0);
        }
    }

    bool push(const T& t)
    {
        auto write = fifo.write(1);
        if (write.blockSize1 > 0)
        {
            buffers[write.startIndex1] = t;
            return true;
        }
        return false;
    }

    bool pull(T& t)
    {
        auto read = fifo.read(1);
        if (read.blockSize1 > 0)
        {
            t = buffers[read.startIndex1];
            return true;
        }

        return false;
    }

    int getNumAvailableForReading() const
    {
        return fifo.getNumReady();
    }

private:
    static constexpr int Capacity = 30;
    std::array<T, Capacity> buffers;
    juce::AbstractFifo fifo{ Capacity };
};

enum Channel
{
    Right, //effectively 0
    Left //effectively 1
};

template<typename BlockType>
struct SingleChannelSampleFifo
{
    SingleChannelSampleFifo(Channel ch) : channelToUse(ch)
    {
        prepared.set(false);
    }

    void update(const BlockType& buffer)
    {
        jassert(prepared.get());
        jassert(buffer.getNumChannels() > channelToUse);
        auto* channelPtr = buffer.getReadPointer(channelToUse);

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            pushNextSampleIntoFifo(channelPtr[i]);
        }
    }

    void prepare(int bufferSize)
    {
        prepared.set(false);
        size.set(bufferSize);

        bufferToFill.setSize(1,         //channel
            bufferSize,                 //num samples
            false,                      //keepExistingContent
            true,                       //clear extra space
            true);                      //avoid reallocating
        audioBufferFifo.prepare(1, bufferSize);
        fifoIndex = 0;
        prepared.set(true);
    }
    //==============================================================================
    int getNumCompleteBuffersAvailable() const { return audioBufferFifo.getNumAvailableForReading(); }
    bool isPrepared() const { return prepared.get(); }
    int getSize() const { return size.get(); }
    //==============================================================================
    bool getAudioBuffer(BlockType& buf) { return audioBufferFifo.pull(buf); }
private:
    Channel channelToUse;
    int fifoIndex = 0;
    Fifo<BlockType> audioBufferFifo;
    BlockType bufferToFill;
    juce::Atomic<bool> prepared = false;
    juce::Atomic<int> size = 0;

    void pushNextSampleIntoFifo(float sample)
    {
        if (fifoIndex == bufferToFill.getNumSamples())
        {
            auto ok = audioBufferFifo.push(bufferToFill);

            juce::ignoreUnused(ok);

            fifoIndex = 0;
        }

        bufferToFill.setSample(0, fifoIndex, sample);
        ++fifoIndex;
    }
};

enum TransportState
{
    Stopped,
    Starting,
    Playing,
    Stopping
};

enum EnvelopeType
{
    ADSR,
    Lineal,
    Sinusoid
};

struct Envelope 
{
    virtual float applyEnvelopeToSample(int totalSamples, int pos) = 0;
};

struct ADSRSettings : public Envelope
{
    float attackPerc{ 0.25f }, decPerc{ 0.25f }, 
        sustPerc{ 0.25f }, relPerc{ 0.25f };

    ADSRSettings() {};

    ADSRSettings(float attack, float decay, float sustain, float release) :
        attackPerc(attack), decPerc(decay), 
        sustPerc(sustain), relPerc(release) {}

    virtual float applyEnvelopeToSample(int totalSamples, int pos)
    {
        juce::int64 totalAtt = attackPerc * totalSamples;
        juce::int64 totalDec = decPerc * totalSamples;
        juce::int64 totalSus = sustPerc * totalSamples;
        juce::int64 totalRel = relPerc * totalSamples;

        juce::int64 totalRight = 0.5f * totalSamples;

        if (pos < totalAtt)
            return float(pos) / totalAtt;
        else if (pos < totalAtt + totalDec)
            return 0.8f + (0.2f * float(totalAtt + totalDec - pos) / totalDec);
        else if (pos < totalAtt + totalDec + totalSus)
            return 0.8f;
        else 
            return 0.8f * float(totalSamples - pos) / totalRel;
    }
};

struct LinealSettings : public Envelope
{
    float leftRange{ 0.5f }, rightRange{ 0.5f };

    LinealSettings() {};

    LinealSettings(float left, float right) : 
        leftRange(left), rightRange(right) {}

    virtual float applyEnvelopeToSample(int totalSamples, int pos)
    {
        juce::int64 totalLeft = leftRange * totalSamples;
        juce::int64 totalRight = rightRange * totalSamples;

        if (pos < totalLeft)
            return float(pos) / totalLeft;
        else if (pos > totalSamples - totalRight)
            return (float(totalSamples - pos)) / totalRight;
        else return 1.f;
    }
};

struct SinusoidSettings : public Envelope
{
    float leftRange{ 0.5f }, rightRange{ 0.5f };

    SinusoidSettings() {}

    SinusoidSettings(float left, float right) :
        leftRange(left), rightRange(right) {}

    virtual float applyEnvelopeToSample(int totalSamples, int pos)
    {
        juce::int64 totalLeft = leftRange * totalSamples;
        juce::int64 totalRight = rightRange * totalSamples;

        if (pos < totalLeft)
            return sinusoidFunction(float(pos) / totalLeft);
        else if (pos > totalSamples - totalRight)
            return sinusoidFunction((float(totalSamples - pos)) / totalRight);
        else return 1.f;
    }

    float sinusoidFunction(float x) {
        return 1.f - cos((x * juce::MathConstants<float>::pi) * 0.5f) + 0.5f;
    }
};

struct GranularSamplerSettings
{
    EnvelopeType envelopeType = EnvelopeType::Lineal;

    float maximumSecondsDuration{ 0 },
        grainDensity{ 0 }, grainMaxLength{ 0 }, grainMinLength{ 0 },
        startingPosMin{ 0 }, startingPosMax{ 0 };
        LinealSettings linealSettings;
        ADSRSettings adsrSettings;

    bool endless{ false };
};
