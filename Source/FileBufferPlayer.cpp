#include "FileBufferPlayer.h"
#include "PluginProcessor.h"

void FileBufferPlayer::changeState(const TransportState newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
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
        audioProcessor->updateState(state);
    }
}

void FileBufferPlayer::copyNextBlockFromBufferFileTo(juce::AudioBuffer<float>& buff)
{
    if (state == Stopping)
    {
        changeState(Stopped);
        return;
    }
    if (!playerPlaying)
        return;
    if (state == Starting)
        changeState(Playing);
    if (buff.getNumChannels() == bufferFile.getNumChannels())
    {
        int numSamples = buff.getNumSamples();
        if (bufferFile.getNumSamples() < numSamples + playerPos)
        {
            changeState(Stopping);
            numSamples = bufferFile.getNumSamples() - playerPos;
        }
        for (int i = 0; i < buff.getNumChannels(); i++)
        {
            buff.copyFrom(i, 0, bufferFile, i, playerPos, numSamples);
        }

        playerPos += numSamples;
    }
}
