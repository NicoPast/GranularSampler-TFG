#include "FileBufferPlayer.h"
#include "PluginProcessor.h"

FileBufferPlayer::FileBufferPlayer(GranularSamplerAudioProcessor* processor) :
    audioProcessor(processor),
    playerPos(0), 
    playerState(Stopped), playerPlaying(false)
{
}

FileBufferPlayer::~FileBufferPlayer()
{
}

void FileBufferPlayer::reset()
{
    playerPos = 0;
    playerState = Stopped;
    playerPlaying = false;
}

TransportState FileBufferPlayer::getState()
{
    return playerState;
}

void FileBufferPlayer::setState(const TransportState newState)
{
    if (playerState != newState)
    {
        playerState = newState;

        switch (playerState)
        {
        case Stopped:                           // [3]
            playerPos = 0;
            break;

        case Starting:                          // [4]
            playerPlaying = true;
            break;

        case Playing:
            break;

        case Stopping:                          // [6]
            playerPlaying = false;
            break;
        default:
            DBG("INVALIS STATE RECIEVED");
            break;
        }
        audioProcessor->updateState(playerState);
    }
}

void FileBufferPlayer::copyNextBlockFromBufferFileTo(juce::AudioBuffer<float>& buff)
{
    if (playerState == Stopping)
    {
        setState(Stopped);
        return;
    }
    if (!playerPlaying)
        return;
    if (buff.getNumChannels() == bufferFile.getNumChannels())
    {
        if (playerState == Starting)
            setState(Playing);
        int numSamples = buff.getNumSamples();
        if (bufferFile.getNumSamples() < numSamples + playerPos)
        {
            setState(Stopping);
            numSamples = bufferFile.getNumSamples() - playerPos;
        }
        for (int i = 0; i < buff.getNumChannels(); i++)
        {
            buff.copyFrom(i, 0, bufferFile, i, playerPos, numSamples);
        }

        playerPos += numSamples;
    }
}

juce::AudioBuffer<float>& FileBufferPlayer::getBuffer()
{
    return bufferFile;
}

void FileBufferPlayer::setBuffer(const juce::AudioBuffer<float>& buff)
{
    bufferFile = buff;
}
