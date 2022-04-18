#pragma once

#include "JuceHeader.h"
#include "CommonProcessor.h"

class GranularSamplerAudioProcessor;

class FileBufferPlayer
{
public:
    FileBufferPlayer(GranularSamplerAudioProcessor* processor);

    ~FileBufferPlayer();

    void reset();

    TransportState getState();

    void setState(const TransportState newState);

    void copyNextBlockFromBufferFileTo(juce::AudioBuffer<float>& buff);

    juce::AudioBuffer<float>& getBuffer();

    void setBuffer(const juce::AudioBuffer<float>& buff);

private:
    GranularSamplerAudioProcessor* audioProcessor;

    juce::AudioBuffer<float> bufferFile;
    juce::int64 playerPos;
    bool playerPlaying;
    TransportState playerState;
};