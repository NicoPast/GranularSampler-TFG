#pragma once

#include "JuceHeader.h"
#include "CommonProcessor.h"

class GranularSamplerAudioProcessor;

class FileBufferPlayer
{
public:
    FileBufferPlayer(GranularSamplerAudioProcessor* processor) :
        audioProcessor(processor),
        playerPos(0), state(Stopped), playerPlaying(false)
    {
    }

    ~FileBufferPlayer()
    {
    }

    void reset()
    {
        playerPos = 0;
        state = Stopped;
        playerPlaying = false;
    }

    TransportState getState()
    {
        return state;
    }

    void changeState(const TransportState newState);

    void copyNextBlockFromBufferFileTo(juce::AudioBuffer<float>& buff);

    juce::AudioBuffer<float>& getBuffer()
    {
        return bufferFile;
    }

    void setBuffer(juce::AudioBuffer<float> buff)
    {
        bufferFile = buff;
    }

private:
    GranularSamplerAudioProcessor* audioProcessor;

    juce::AudioBuffer<float> bufferFile;
    juce::int64 playerPos;
    bool playerPlaying;
    TransportState state;
};