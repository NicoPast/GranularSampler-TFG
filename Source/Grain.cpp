/*
  ==============================================================================

    Grain.cpp
    Created: 27 Apr 2022 4:08:53pm
    Author:  aGoat

  ==============================================================================
*/

#include "Grain.h"

Grain::Grain() : grainBuffer(2, 0), startingSample(0), adsr(), currentPos(0), endPos(0)
{
    grainBuffer.clear();
}

Grain::Grain(juce::AudioBuffer<float> buff) : grainBuffer(buff)
{
    applyADSR();
}

Grain::Grain(juce::AudioBuffer<float> buff, juce::int64 originPos, juce::int64 staPos, juce::int64 numSamples)
{
    GranularSamplerSettings settings;
    resetGrain(buff, originPos, staPos, numSamples, settings);
}

Grain::~Grain()
{

}

void Grain::resetGrain(juce::AudioBuffer<float> buff, juce::int64 originPos, juce::int64 staPos, juce::int64 numSamples, GranularSamplerSettings& settings)
{
    grainBuffer.setSize(buff.getNumChannels(), numSamples);

    for (int i = 0; i < buff.getNumChannels(); i++)
    {
        grainBuffer.copyFrom(i, 0, buff, 0, originPos, numSamples);
    }

    startingSample = originPos;
    startPos = staPos;
    currentPos = startPos;
    endPos = numSamples;

    juce::int64 total = endPos - staPos;

    setADSR(44100.0, total * settings.attackPerc, total * settings.decPerc,
        total * settings.sustPerc, total * settings.relPerc);
}

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
    adsr.setSampleRate(sampleRate);

    juce::ADSR::Parameters param(attackSamp / sampleRate, decaySamp / sampleRate,
        sustSamp / sampleRate, relSamp / sampleRate);
    adsr.setParameters(param);

    //peta si startPos > relSamp
    adsr.noteOn();
    adsr.applyEnvelopeToBuffer(grainBuffer, startPos, endPos - startPos - relSamp);
    adsr.noteOff();
    adsr.applyEnvelopeToBuffer(grainBuffer, endPos - relSamp, relSamp);
}

bool Grain::advanceGrainStartPos(juce::int64 playedSamples)
{
    currentPos += playedSamples;
    return currentPos >= endPos;
}

void Grain::setBuffer(const juce::AudioBuffer<float>& buff)
{
    grainBuffer = buff;
    applyADSR();
}

juce::AudioBuffer<float>& Grain::getBuffer()
{
    return grainBuffer;
}

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
    if (currentPos == startPos)
        return startPos;
    return 0;
}

juce::int64 Grain::getRemainingSamples()
{
    return endPos - currentPos;
}

void Grain::clear()
{
    grainBuffer.clear();
    startingSample = 0;

    startPos = 0;
    currentPos = 0;
    endPos = 0;
}

void Grain::applyADSR()
{
    adsr.applyEnvelopeToBuffer(grainBuffer, startPos, endPos);
    //for (int i = 0; i < grainBuffer.getNumSamples(); i++)
    //{
    //    DBG(grainBuffer.getSample(0, i));
    //}
}