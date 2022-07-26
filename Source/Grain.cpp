/*
  ==============================================================================

    Grain.cpp
    Created: 27 Apr 2022 4:08:53pm
    Author:  aGoat

  ==============================================================================
*/

#include "Grain.h"

Grain::Grain() : grainBufferBlock(2,0), startingSample(0), adsr(), currentPos(0), endPos(0)
{
    grainBufferBlock.clear();
}

//Grain::Grain(juce::AudioBuffer<float> buff) : grainBuffer(buff)
//{
//    applyADSR();
//}

Grain::Grain(juce::AudioBuffer<float> buff, juce::int64 originPos, juce::int64 staPos, juce::int64 numSamples)
{
    GranularSamplerSettings settings;
    resetGrain(buff, originPos, staPos, numSamples, settings);
}

Grain::~Grain()
{
    if (envelope != nullptr)
    {
        delete envelope;
        envelope = nullptr;
    }
}

void Grain::init(int numChannels, int numSamples)
{
    grainBufferBlock.setSize(numChannels, numSamples);
}

void Grain::resetGrain(juce::AudioBuffer<float> buff, 
    juce::int64 originPos, juce::int64 staPos, juce::int64 numSamples, 
    GranularSamplerSettings& settings)
{
    //grainBuffer.setSize(buff.getNumChannels(), numSamples);

    //for (int i = 0; i < buff.getNumChannels(); i++)
    //{
    //    grainBuffer.copyFrom(i, 0, buff, 0, originPos, numSamples);
    //}

    if (envelope != nullptr)
    {
        delete envelope;
        envelope = nullptr;
    }

    startingSample = originPos;
    startPos = staPos;
    currentPos = startPos;
    endPos = numSamples;

    totalSamples = endPos - staPos;

    switch (settings.envelopeType)
    {
    case EnvelopeType::ADSR:
        envelope = new ADSRSettings(settings.adsrSettings.attackPerc,
            settings.adsrSettings.decPerc,
            settings.adsrSettings.sustPerc,
            settings.adsrSettings.relPerc);
        DBG("ADSR");
        break;
    case EnvelopeType::Lineal:
        envelope = new LinealSettings(settings.linealSettings.leftRange,
            settings.linealSettings.rightRange);
        DBG("LINEAL");
        break;
    case EnvelopeType::Sinusoid:
        break;
    default:
        break;
    }

    // settings para los dos valores
    //setLinear(totalSamples, 0.5, 0.5);

    //setADSR(44100.0, total * settings.attackPerc, total * settings.decPerc,
    //    total * settings.sustPerc, total * settings.relPerc);
}

// old
//void Grain::setADSR(double sampleRate, float attackSecs, float decaySecs,
//    float sustSecs, float relSecs)
//{
//    adsr.setSampleRate(sampleRate);
//
//    juce::ADSR::Parameters param(attackSecs, decaySecs, sustSecs, relSecs);
//    adsr.setParameters(param);
//
//    adsr.applyEnvelopeToBuffer(grainBuffer, startPos, endPos);
//}

void Grain::setADSR(double sampleRate, juce::int64 attackSamp, juce::int64 decaySamp,
    juce::int64 sustSamp, juce::int64 relSamp)
{
    //adsr.setSampleRate(sampleRate);

    //juce::ADSR::Parameters param(attackSamp / sampleRate, decaySamp / sampleRate,
    //    sustSamp / sampleRate, relSamp / sampleRate);
    //adsr.setParameters(param);

    ////peta si startPos > relSamp
    //adsr.noteOn();
    //adsr.applyEnvelopeToBuffer(grainBuffer, startPos, endPos - startPos - relSamp);
    //adsr.noteOff();
    //adsr.applyEnvelopeToBuffer(grainBuffer, endPos - relSamp, relSamp);
}

void Grain::setLinear(juce::int64 total, float leftRange, float rightRange)
{
    //auto numChannels = grainBuffer.getNumChannels();

    //juce::int64 totalLeft = leftRange * total;

    //for (juce::int64 i = 0; i < totalLeft; ++i)
    //{
    //    for (int j = 0; j < numChannels; ++j)
    //        grainBuffer.getWritePointer(j)[startPos + i] *= float(i) / totalLeft;
    //}

    //juce::int64 totalRight = rightRange * total;

    //for (juce::int64 i = 0; i < totalRight; ++i)
    //{
    //    for (int j = 0; j < numChannels; ++j)
    //        grainBuffer.getWritePointer(j)[endPos - i] *= float(i) / totalRight;
    //}
}

float Grain::applyEnvelopeToSample(int i)
{
    // TODO: more than just lineal
    int pos = currentPos + i;

    if(envelope)
        return envelope->applyEnvelopeToSample(totalSamples, pos);

    //if there is no envelope, do this one as default
    juce::int64 totalLeft = 0.5f * totalSamples;
    juce::int64 totalRight = 0.5f * totalSamples;

    if (pos < totalLeft)
        return float(pos) / totalLeft;
    else if (pos > totalSamples - totalRight)
        return (float(totalSamples - pos)) / totalRight;
    else return 1.f;
}


bool Grain::advanceGrainStartPos(juce::int64 playedSamples)
{
    currentPos += playedSamples;
    return currentPos >= endPos;
}

//void Grain::setBuffer(const juce::AudioBuffer<float>& buff)
//{
//    grainBuffer = buff;
//    applyADSR();
//}

//juce::AudioBuffer<float>& Grain::getBuffer()
//{
//    return grainBuffer;
//}

juce::int64 Grain::getStartPos()
{
    return startPos;
}

juce::int64 Grain::getCurrentPos()
{
    return currentPos;
}

juce::int64 Grain::getEndPos()
{
    return endPos;
}

juce::int64 Grain::getFirstSample()
{
    // if it's in the beginning, startPos, else 0
    if (currentPos == startPos)
        return startPos;
    return 0;
}

juce::int64 Grain::getRemainingSamples()
{
    return endPos - currentPos;
}

juce::AudioBuffer<float>& Grain::getNextBufferBlock(const juce::AudioBuffer<float>& buffFile,
    const juce::AudioBuffer<float>& buffBlock)
{
    // the bufferblock will be the minimum between
    // the size of buffer block or the remaining samples to play
    int samples = juce::jmin<juce::int64>(buffBlock.getNumSamples(),
        getRemainingSamples());
    int channels = buffBlock.getNumChannels();

    //grainBufferBlock.setSize(channels, samples);

    // for every channel in the block's buffer
    for (int ch = 0; ch < channels; ++ch)
    {
        auto blockPointer = grainBufferBlock.getWritePointer(ch);
        auto filePointer = buffFile.getReadPointer(ch);

        // read from the file and store it in the block's buffer
        for (int i = 0; i < samples - getFirstSample(); ++i)
        {
            //apply envelope if necesary
            float samp = filePointer[startingSample + currentPos + i] * applyEnvelopeToSample(i);
            
            blockPointer[getFirstSample() + i] = samp;
        }
    }

    return grainBufferBlock;
}

void Grain::clear()
{
    //grainBuffer.clear();
    //grainBufferBlock.clear();

    startingSample = 0;

    startPos = 0;
    currentPos = 0;
    endPos = 0;
}