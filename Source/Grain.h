/*
  ==============================================================================

    Grain.h
    Created: 27 Apr 2022 4:08:53pm
    Author:  aGoat

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "CommonProcessor.h"

class Grain
{
public:
    Grain();
    //Grain(juce::AudioBuffer<float> buff);
    Grain(juce::AudioBuffer<float> buff, juce::int64 originPos, juce::int64 staPos, juce::int64 numSamples);
    ~Grain();

    void resetGrain(juce::AudioBuffer<float> buff, juce::int64 originPos,
        juce::int64 staPos, juce::int64 numSamples,
        GranularSamplerSettings& settings);

    //void setADSR(double sampleRate, float attackSecs, float decaySecs,
    //    float sustSecs, float relSecs);

    void setADSR(double sampleRate, juce::int64 attackSamp, juce::int64 decaySamp,
        juce::int64 sustSamp, juce::int64 relSamp);

    bool advanceGrainStartPos(juce::int64 playedSamples);

    //void setBuffer(const juce::AudioBuffer<float>& buff);

    //juce::AudioBuffer<float>& getBuffer();

    juce::int64 getStartPos();

    juce::int64 getCurrentPos();

    juce::int64 getEndPos();

    juce::int64 getFirstSample();

    juce::int64 getRemainingSamples();

    juce::AudioBuffer<float>& getNextBufferBlock(const juce::AudioBuffer<float>& buffFile,
        const juce::AudioBuffer<float>& buffBlock);

    void clear();
private:
    void applyADSR();

    void setLinear(juce::int64 total, float leftRange, float rightRange);

    float applyEnvelopeToSample(int i);

    //juce::AudioBuffer<float> grainBuffer;
    juce::AudioBuffer<float> grainBufferBlock;

    // position from the audio buffer of the file
    juce::int64 startingSample;

    juce::ADSR adsr;

    // random variation of Grain

    // starting position of the grain Buffer
    juce::int64 startPos = 0;
    // current playing position
    juce::int64 currentPos;
    // endposition
    juce::int64 endPos;
    // total of samples
    juce::int64 totalSamples = 0;

    // TODO: ????? lo hago?
    float paning = 0; // -1.f left, 0 centered, 1.f right
    float pitch = 0; // altered pitch of the Grain
};