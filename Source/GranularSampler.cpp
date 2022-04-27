/*
  ==============================================================================

    GranularSampler.cpp
    Created: 27 Apr 2022 4:39:49pm
    Author:  aGoat

  ==============================================================================
*/

#include "GranularSampler.h"
#include "PluginProcessor.h"

GranularSampler::GranularSampler(GranularSamplerAudioProcessor* audioProc) : audioProcessor(audioProc)
{
    for (int i = 0; i < poolSize; i++)
    {
        grainPool.push_back(new Grain());
    }
    //grainPool = std::list<Grain>(poolSize);
}

GranularSampler::~GranularSampler()
{
    while (!playingGrain.empty())
    {
        delete playingGrain.front();
        playingGrain.pop_front();
    }
    while (!grainPool.empty())
    {
        delete grainPool.front();
        grainPool.pop_front();
    }
}

void GranularSampler::getNextAudioBlock(juce::AudioBuffer<float>& buffer)
{
    // usar el sistema de estado del player aqui tambien
    if (samplerState == Stopping)
    {
        setState(Stopped);
        return;
    }
    if (!samplerPlaying)
        return;

    if (samplerState == Starting)
        setState(Playing);

    int numSamples = buffer.getNumSamples();
    if (totalNumSamples < numSamples + samplerPos)
    {
        setState(Stopping);
        numSamples = totalNumSamples - samplerPos;
    }

    //dame nuevos granos en funcion de la densidad

    if (fileBuff != nullptr)
    {
        float gDens = JUCE_LIVE_CONSTANT(20.f);

        // TODO: que la densidad funcione con floats en vez de con enteros
        while (playingGrain.size() < gDens && !grainPool.empty())
        {
            juce::int64 grainSize = juce::Random::getSystemRandom().nextFloat()
                * (buffer.getNumSamples() * JUCE_LIVE_CONSTANT(80.5f) - 1000) + 1000;

            grainSize *= 10;

            juce::int64 randomFilePos = juce::Random::getSystemRandom()
                .nextInt(fileBuff->getBuffer().getNumSamples() - grainSize);

            juce::int64 size = juce::jmin<juce::int64>(numSamples,
                grainSize);
            juce::int64 randomStartPos = juce::Random::getSystemRandom()
                .nextInt(size);
            Grain* g = grainPool.front();
            grainPool.pop_front();
            g->resetGrain(fileBuff->getBuffer(), randomFilePos, randomStartPos, grainSize);
            playingGrain.push_back(g);
            //playingGrain.push_back(new Grain(fileBuff->getBuffer(), randomFilePos, randomStartPos, grainSize));
        }

        int played = 0;

        std::list<Grain*>::iterator it = playingGrain.begin();
        while (it != playingGrain.end())
        {
            //metelos en el buffer
            juce::int64 samples = juce::jmin<juce::int64>(numSamples,
                (*it)->getRemainingSamples());
            juce::int64 startingPos = (*it)->getFirstSample();
            for (int i = 0; i < buffer.getNumChannels(); i++)
            {
                buffer.addFrom(i, startingPos, (*it)->getBuffer(), i, (*it)->getCurrentPos(), samples - startingPos);
            }

            if ((*it)->advanceGrainStartPos(samples))
            {
                grainPool.push_back(*it);
                (*it)->clear();
                //delete (*it);
                playingGrain.erase(it++);
            }
            else ++it;

            played++;
        }

        // TODO: normalizado funciona asi asa
        // funciona asi porque esta leyendo el inmediato solo, y deberia en vez de normalizarlo
        // bajar la ganancia a todos los buffers en vez de normalizar
        //normalizamos el resultado
        //juce::Range<float> minMax = buffer.findMinMax(0, 0, buffer.getNumSamples());
        ////solo hazlo si el canal lo necesita
        //if (minMax.getStart() < -1.f || minMax.getEnd() > 1.f)
        //{
        //    juce::Range<float> fileMinMax = fileBuff->getMinMaxValuesLeftChannel();
        //    for (int i = 0; i < numSamples; i++) {
        //        float scaled = scaleBetween(buffer.getSample(0, i),
        //            fileMinMax.getStart(), fileMinMax.getEnd(),
        //            minMax.getStart(), minMax.getEnd());
        //        buffer.setSample(0, i, scaled);
        //    }
        //    //buffer.applyGain(0, 0, buffer.getNumSamples(), 1.f / played);
        //}
        //minMax = buffer.findMinMax(1, 0, buffer.getNumSamples());
        //if (minMax.getStart() < -1.f || minMax.getEnd() > 1.f)
        //{
        //    juce::Range<float> fileMinMax = fileBuff->getMinMaxValuesRightChannel();
        //    for (int i = 0; i < numSamples; i++) {
        //        float scaled = scaleBetween(buffer.getSample(1, i),
        //            fileMinMax.getStart(), fileMinMax.getEnd(),
        //            minMax.getStart(), minMax.getEnd());
        //        buffer.setSample(1, i, scaled);
        //    }
        //}

        // TODO: be more clever than this
        // quizas debería implementar un compresor?
        float strength = 2.f;

        buffer.applyGain(strength * 1.f / played);

        //DBG(buffer.findMinMax(0, 0, buffer.getNumSamples()).getEnd());

        samplerPos += numSamples;
    }
}

void GranularSampler::updateADSRPercs(float att, float dec, float sus, float rel)
{

}

void GranularSampler::setState(TransportState newState)
{
    if (samplerState != newState)
    {
        samplerState = newState;

        switch (samplerState)
        {
        case Stopped:                           // [3]
            samplerPos = 0;
            while (!playingGrain.empty())
            {
                grainPool.push_back(playingGrain.front());
                playingGrain.front()->clear();
                playingGrain.pop_front();
            }
            break;

        case Starting:                          // [4]
            samplerPlaying = true;
            break;

        case Playing:
            break;

        case Stopping:                          // [6]
            samplerPlaying = false;
            break;
        default:
            DBG("INVALIS STATE RECIEVED");
            break;
        }
        audioProcessor->updateSamplerState(samplerState);
    }
}

TransportState GranularSampler::getState()
{
    return samplerState;
}

FileBufferPlayer* GranularSampler::getFileBuffer()
{
    return fileBuff;
}

void GranularSampler::setFileBuffer(FileBufferPlayer* fBuff)
{
    fileBuff = fBuff;
    totalNumSamples = fileBuff->getBuffer().getNumSamples();
}

float GranularSampler::scaleBetween(float unscaledNum, float minAllowed, float maxAllowed, float min, float max) {
    return (maxAllowed - minAllowed) * (unscaledNum - min) / (max - min) + minAllowed;
}