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
        changeState(Stopped);
        return;
    }
    if (!samplerPlaying)
        return;

    if (samplerState == Starting)
        changeState(Playing);

    GranularSamplerSettings settings = getGranularSamplerSettings(audioProcessor->apvts);

    int numSamples = buffer.getNumSamples();
    if (settings.endless && totalNumSamples < numSamples + samplerPos)
    {
        changeState(Stopping);
        numSamples = totalNumSamples - samplerPos;
    }

    //dame nuevos granos en funcion de la densidad
    if (fileBuff != nullptr)
    {
        // TODO: que la densidad funcione con floats en vez de con enteros
        while (playingGrain.size() < settings.grainDensity && !grainPool.empty())
        {
            float gMaxLeng = settings.grainMaxLength;
            float gMinLeng = settings.grainMinLength;
            
            //juce::int64 grainSize = juce::Random::getSystemRandom().nextFloat()
            //    * (JUCE_LIVE_CONSTANT(20.5f) * audioProcessor->getSampleRate() - numSamples) + numSamples;

            //grainSize in range of [gMinLeng, gMaxLeng)
            juce::int64 grainSize = (juce::Random::getSystemRandom().nextFloat() 
                * (gMaxLeng - gMinLeng) + gMinLeng) 
                * audioProcessor->getSampleRate() + buffer.getNumSamples();

            // avoids a grain bigger than the original sample
            if (grainSize > fileBuff->getBuffer().getNumSamples())
                grainSize = fileBuff->getBuffer().getNumSamples();

            juce::int64 randomFilePos = (juce::Random::getSystemRandom().nextFloat() 
                * (settings.startingPosMax - settings.startingPosMin) + settings.startingPosMin)
                / 100.f
                * (fileBuff->getBuffer().getNumSamples() - grainSize);

            juce::int64 size = juce::jmin<juce::int64>(numSamples,
                grainSize);
            juce::int64 randomStartPos = juce::Random::getSystemRandom()
                .nextInt(size);
            Grain* g = grainPool.front();
            grainPool.pop_front();
            g->resetGrain(fileBuff->getBuffer(), randomFilePos, randomStartPos, grainSize, settings);
            playingGrain.push_back(g);
            //playingGrain.push_back(new Grain(fileBuff->getBuffer(), randomFilePos, randomStartPos, grainSize));
        }

        int played = playingGrain.size();

        //DBG(played);

        std::list<Grain*>::iterator it = playingGrain.begin();
        while (it != playingGrain.end())
        {
            //metelos en el buffer
            juce::int64 samples = juce::jmin<juce::int64>(numSamples,
                (*it)->getRemainingSamples());
            juce::int64 startingPos = (*it)->getFirstSample();
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                //buffer.addFrom(ch, startingPos, 
                //    (*it)->getBuffer(), ch, (*it)->getCurrentPos(), samples - startingPos,
                //    1.f/played);

                buffer.addFrom(ch, startingPos,
                    (*it)->getNextBufferBlock(fileBuff->getBuffer(), buffer),
                    ch, startingPos, samples - startingPos, 1.f / played);
            }

            if ((*it)->advanceGrainStartPos(samples))
            {
                //delete (*it);
                grainPool.push_front(*it);
                (*it)->clear();
                playingGrain.erase(it++);
            }
            else ++it;

            //played++;
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
        float strength = 1.f;

        //buffer.applyGain(strength * 1.f / played);

        //DBG(played);

        //DBG(buffer.findMinMax(0, 0, buffer.getNumSamples()).getEnd());

        samplerPos += numSamples;
    }
}

void GranularSampler::updateADSRPercs(float att, float dec, float sus, float rel)
{

}

void GranularSampler::changeState(TransportState newState)
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
            DBG("INVALID STATE RECIEVED");
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

void GranularSampler::prepareGrains(int numChannels, int numSamples)
{
    for (auto it : grainPool)
    {
        (*it).init(numChannels, numSamples);
    }
}

float GranularSampler::scaleBetween(float unscaledNum, float minAllowed, float maxAllowed, float min, float max) {
    return (maxAllowed - minAllowed) * (unscaledNum - min) / (max - min) + minAllowed;
}

GranularSamplerSettings getGranularSamplerSettings(juce::AudioProcessorValueTreeState& apvts)
{
    GranularSamplerSettings settings;
    
    //maximumSecondsDuration
    settings.grainDensity = apvts.getRawParameterValue("Grain Density")->load() / 100.f;
    settings.grainMinLength = apvts.getRawParameterValue("Grain Min Length")->load();
    settings.grainMaxLength = apvts.getRawParameterValue("Grain Max Length")->load();
    settings.startingPosMin = apvts.getRawParameterValue("Grain Min StartPos")->load();
    settings.startingPosMax = apvts.getRawParameterValue("Grain Max StartPos")->load();
    //lineal
    settings.linealSettings.leftRange = apvts.getRawParameterValue("Grain Lin Left")->load() / 100.f;
    settings.linealSettings.rightRange = 1.f - apvts.getRawParameterValue("Grain Lin Left")->load() / 100.f;
    //adsr
    settings.adsrSettings.attackPerc = apvts.getRawParameterValue("Grain ADSR Attack")->load() / 100.f;
    settings.adsrSettings.decPerc = apvts.getRawParameterValue("Grain ADSR Decay")->load() / 100.f - settings.adsrSettings.attackPerc;
    settings.adsrSettings.sustPerc = apvts.getRawParameterValue("Grain ADSR Sustain")->load() / 100.f - (settings.adsrSettings.attackPerc + settings.adsrSettings.decPerc);
    settings.adsrSettings.relPerc = 1.f - (settings.adsrSettings.attackPerc + settings.adsrSettings.decPerc + settings.adsrSettings.sustPerc);

    settings.endless = apvts.getRawParameterValue("Endless")->load() > 0.5f;

    settings.envelopeType = static_cast<EnvelopeType>(apvts.getRawParameterValue("Grain Envelope Type")->load());

    return settings;
}
