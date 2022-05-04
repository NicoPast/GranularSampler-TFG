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
    GranularSampler(GranularSamplerAudioProcessor* audioProc);
    ~GranularSampler();

    void getNextAudioBlock(juce::AudioBuffer<float>& buffer);

    void updateADSRPercs(float att, float dec, float sus, float rel);

    void changeState(TransportState newState);

    TransportState getState();

    FileBufferPlayer* getFileBuffer();

    void setFileBuffer(FileBufferPlayer* fBuff);

private:
    float scaleBetween(float unscaledNum, float minAllowed, float maxAllowed, float min, float max);

    GranularSamplerAudioProcessor* audioProcessor;
    FileBufferPlayer* fileBuff = nullptr;

    TransportState samplerState;
    bool samplerPlaying = false;

    float totalSecsPlayingTime;
    juce::int64 samplerPos = 0;
    juce::int64 totalNumSamples;

    //float grainDensity;
    //float grainDuration;

    float attackPerc;
    float decayPerc;
    float sustPerc;
    float relPerc;

    float startingPositionRandomMin;
    float startingPositionRandomMax;
    float densityRandom;
    float panRandom;
    float pitchRandom;

    // unaltered grains which will still be playing
    std::list<Grain*> playingGrain;

    std::list<Grain*> grainPool;
    int poolSize = 100;
};