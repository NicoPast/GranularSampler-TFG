/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CommonProcessor.h"
#include "EQProcessor.h"
#include "FileBufferPlayer.h"

class Grain
{
public:
    Grain() {}
    Grain(juce::AudioBuffer<float> buff) : grainBuffer(buff)
    {
        applyADSR();
    }
    Grain(juce::AudioBuffer<float> buff, juce::int64 startPos, juce::int64 numSamples)
    {
        grainBuffer.setSize(buff.getNumChannels(), numSamples);
        
        for (int i = 0; i < buff.getNumChannels(); i++)
        {
            grainBuffer.copyFrom(i, 0, buff, 0, startPos, numSamples);
        }

        startingSample = startPos;
        endPos = numSamples;

        setADSR(44100.0, numSamples/4, numSamples / 4, numSamples / 4, numSamples / 4);
    }
    ~Grain() {}

    void setADSR(double sampleRate, float attackSecs, float decaySecs,
        float sustSecs, float relSecs)
    {
        adsr.setSampleRate(sampleRate);
        
        juce::ADSR::Parameters param(attackSecs, decaySecs, sustSecs, relSecs);
        adsr.setParameters(param);

        adsr.applyEnvelopeToBuffer(grainBuffer, startPos, endPos);
    }

    void setADSR(double sampleRate, juce::int64 attackSamp, juce::int64 decaySamp,
        juce::int64 sustSamp, juce::int64 relSamp)
    {
        adsr.setSampleRate(sampleRate);

        juce::ADSR::Parameters param(attackSamp / sampleRate, decaySamp / sampleRate,
            sustSamp / sampleRate, relSamp / sampleRate);
        adsr.setParameters(param);

        adsr.noteOn();
        adsr.applyEnvelopeToBuffer(grainBuffer, startPos, endPos);
        adsr.noteOff();
        adsr.applyEnvelopeToBuffer(grainBuffer, endPos - decaySamp, relSamp);
    }
    
    void cutAlreadyPlayedSamples(juce::int64 currentSample, juce::int64 playedSamples)
    {

    }

    void setBuffer(const juce::AudioBuffer<float>& buff)
    {
        grainBuffer = buff;
        applyADSR();
    }

    juce::AudioBuffer<float>& getBuffer()
    {
        return grainBuffer;
    }

    juce::int64 getStartPos()
    {
        return startPos;
    }

    juce::int64 getEndPos()
    {
        return endPos;
    }

private:
    void applyADSR()
    {
        adsr.applyEnvelopeToBuffer(grainBuffer, startPos, endPos);
        //for (int i = 0; i < grainBuffer.getNumSamples(); i++)
        //{
        //    DBG(grainBuffer.getSample(0, i));
        //}
    }

    juce::AudioBuffer<float> grainBuffer;
    juce::int64 startingSample;

    juce::ADSR adsr;

    // random variation of Grain
    juce::int64 startPos = 0;
    juce::int64 endPos;
    // TODO: ????? lo hago?
    float paning; // -1.f left, 0 centered, 1.f right
    float pitch; // altered pitch of the Grain
};

class GranularSampler
{
// idea para la densidad
// se tira un d100, si la densidad es mayor a 100, seguro se coge uno
// se le resta 100 a la densidad
// mientras no falle, sigue calculando un nuevo grano
// los granos persistentes, restan 100 por cada

// idea 2 para la densidad
// si es 100 recorres todo el bloque y te aseguras que suena siempre
// restas 100, si es mas de 100 repites
// si es menos de 100, lo utilizo como un minimo de porcentaje que debo ocupar

public:
    GranularSampler() {}
    ~GranularSampler() {}

    void getNextAudioBlock(juce::AudioBuffer<float>& buffer)
    {
        // usar el sistema de estado del player aqui tambien
        if (samplerState == Stopping)
        {
            setState(Stopped);
            return;
        }
        if (!samplerPlaying)
            return;

        if (samplerState == Starting)
            setState(Playing);

        int numSamples = buffer.getNumSamples();
        if (totalNumSamples < numSamples + samplerPos)
        {
            setState(Stopping);
            numSamples = totalNumSamples - samplerPos;
        }

        //dame nuevos granos

        juce::int64 size = buffer.getNumSamples();

        if (fileBuff != nullptr)
        {
            juce::int64 randomPos = juce::Random::getSystemRandom()
                .nextInt(fileBuff->getBuffer().getNumSamples() - size);

            Grain g(fileBuff->getBuffer(), randomPos, size);

            //de todos los granos metelos en el buffer
            
            for (int i = 0; i < buffer.getNumChannels(); i++)
            {
                buffer.copyFrom(i, 0, g.getBuffer(), i, 0, numSamples);
            }
        }

        //normaliza el resultado

        samplerPos += numSamples;
    }

    void updateADSRPercs(float att, float dec, float sus, float rel)
    {

    }

    void setState(TransportState newState)
    {
        if (samplerState != newState)
        {
            samplerState = newState;

            switch (samplerState)
            {
            case Stopped:                           // [3]
                samplerPos = 0;
                break;

            case Starting:                          // [4]
                samplerPlaying = true;
                break;

            case Playing:
                break;

            case Stopping:                          // [6]
                samplerPlaying = false;
                break;
            default:
                DBG("INVALIS STATE RECIEVED");
                break;
            }
            //audioProcessor->updateSamplerState(samplerState);
        }
    }

    TransportState getState()
    {
        return samplerState;
    }

    FileBufferPlayer* getFileBuffer()
    {
        return fileBuff;
    }

    void setFileBuffer(FileBufferPlayer* fBuff)
    {
        fileBuff = fBuff;
        totalNumSamples = fileBuff->getBuffer().getNumSamples();
    }

private:
    //GranularSamplerAudioProcessor* audioProcessor;
    FileBufferPlayer* fileBuff = nullptr;
    
    TransportState samplerState;
    bool samplerPlaying = false;

    float totalSecsPlayingTime;
    juce::int64 samplerPos = 0;
    juce::int64 totalNumSamples;

    float grainDensity;
    float grainDuration;

    float attackPerc;
    float decayPerc;
    float sustPerc;
    float relPerc;

    float startingPositionRandom;
    float densityRandom;
    float panRandom;
    float pitchRandom;

    // unaltered grains which will still be playing
    std::list<Grain> playingGrain;
};

//==============================================================================
/**
*/
class GranularSamplerAudioProcessor  : public juce::AudioProcessor, 
                                       public juce::ChangeListener
{
public:
    //==============================================================================
    GranularSamplerAudioProcessor();
    ~GranularSamplerAudioProcessor() override;

    //==============================================================================
    // main 1 = start();
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    // main 2 = update();
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================

    //==============================================================================
    //================================== My new code ===============================
    //==============================================================================


    // ================================= Parameters ================================
    static juce::AudioProcessorValueTreeState::ParameterLayout
        createParameterLayout();

    // coordinate GUI with real params
    juce::AudioProcessorValueTreeState apvts { *this,
        nullptr, "Parameters", createParameterLayout() };

    using BlockType = juce::AudioBuffer<float>;
    SingleChannelSampleFifo<BlockType> leftChannelFifo{ Channel::Left };
    SingleChannelSampleFifo<BlockType> rightChannelFifo{ Channel::Right };
    
    // =============================================================================

private:
    MonoChain leftChain, rightChain;

    //==============================================================================

#pragma region SamplerPlayer
public:
    bool createReaderFor(juce::File f);

    void changeState(const TransportState newState);

    void updateSamplerState(const TransportState newState);

    void updatePlayerState(const TransportState newState);

    TransportState getTransportState();
private:
    //TransportState transpState;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::AudioSourcePlayer playerAudioSource;

    FileBufferPlayer buffPlay;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

#pragma endregion SamplerPlayer

    // TODO:
    // buffer comodo con todo el audio (quizas tenga que rehacer el Sampler) -> DONE
    // 
    //==============================================================================

#pragma region GranularSampler

    GranularSampler granularSampler;

    // grain Duration

    // Density of Grain
    // Number of Grains
    // Duration of reproduction

    // tipo de envolvente (de base solo OSC, pero puedo añadir ADR tambien y existe Gaussinana)

    // posibilidad de que tenga variacion aleatoria?:
    //      duration
    //      pan
    //      pitch
    //      

#pragma endregion GranularSampler

    //==============================================================================

#pragma region EQMethods

    void updatePeakFilter(const ChainSettings& chainSettings);

    void updateLowCutFilters(const ChainSettings& chainSettings);
    void updateHighCutFilters(const ChainSettings& chainSettings);

    void updateFilters();

#pragma endregion EQMethods

    //==============================================================================

    // for debugging
    juce::dsp::Oscillator<float> osc;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GranularSamplerAudioProcessor)
};