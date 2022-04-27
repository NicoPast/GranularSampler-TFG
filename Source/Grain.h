/*
  ==============================================================================

    Grain.h
    Created: 27 Apr 2022 4:08:53pm
    Author:  aGoat

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class Grain
{
public:
    Grain();
    Grain(juce::AudioBuffer<float> buff);
    Grain(juce::AudioBuffer<float> buff, juce::int64 originPos, juce::int64 staPos, juce::int64 numSamples);
    ~Grain();

    void resetGrain(juce::AudioBuffer<float> buff, juce::int64 originPos, juce::int64 staPos, juce::int64 numSamples);

    void setADSR(double sampleRate, float attackSecs, float decaySecs,
        float sustSecs, float relSecs);

    void setADSR(double sampleRate, juce::int64 attackSamp, juce::int64 decaySamp,
        juce::int64 sustSamp, juce::int64 relSamp);

    bool advanceGrainStartPos(juce::int64 playedSamples);

    void setBuffer(const juce::AudioBuffer<float>& buff);

    juce::AudioBuffer<float>& getBuffer();

    juce::int64 getStartPos();

    juce::int64 getCurrentPos();

    juce::int64 getEndPos();

    juce::int64 getFirstSample();

    juce::int64 getRemainingSamples();

    void clear();
private:
    void applyADSR();

    juce::AudioBuffer<float> grainBuffer;
    juce::int64 startingSample;

    juce::ADSR adsr;

    // random variation of Grain
    juce::int64 startPos = 0;
    juce::int64 currentPos;
    juce::int64 endPos;
    // TODO: ????? lo hago?
    float paning = 0; // -1.f left, 0 centered, 1.f right
    float pitch = 0; // altered pitch of the Grain
};