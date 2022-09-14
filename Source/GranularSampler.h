/*
  ==============================================================================

    GranularSampler.h
    Created: 27 Apr 2022 4:39:49pm
    Author:  aGoat

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Grain.h"
#include "CommonProcessor.h"
#include "FileBufferPlayer.h"

GranularSamplerSettings getGranularSamplerSettings(juce::AudioProcessorValueTreeState& apvts);

class GranularSamplerAudioProcessor;

class GranularSampler
{
public:
    GranularSampler(GranularSamplerAudioProcessor* audioProc);
    ~GranularSampler();

    void getNextAudioBlock(juce::AudioBuffer<float>& buffer);

    void updateADSRPercs(float att, float dec, float sus, float rel);

    void changeState(TransportState newState);

    TransportState getState();

    FileBufferPlayer* getFileBuffer();

    void setFileBuffer(FileBufferPlayer* fBuff);

    void prepareGrains(int numChannels, int numSamples);

private:
    float scaleBetween(float unscaledNum, float minAllowed, float maxAllowed, float min, float max);

    GranularSamplerAudioProcessor* audioProcessor;
    FileBufferPlayer* fileBuff = nullptr;

    TransportState samplerState;
    bool samplerPlaying = false;

    float totalSecsPlayingTime;
    juce::int64 samplerPos = 0;
    juce::int64 totalNumSamples;

    float attackPerc;
    float decayPerc;
    float sustPerc;
    float relPerc;

    float startingPositionRandomMin;
    float startingPositionRandomMax;
    float densityRandom;

    // unaltered grains which will still be playing
    std::list<Grain*> playingGrain;

    std::list<Grain*> grainPool;
    int poolSize = 100;
};