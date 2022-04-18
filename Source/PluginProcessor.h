/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CommonProcessor.h"
#include "EQProcessor.h"
#include "FileBufferPlayer.h"

class Grain
{
public:
    Grain() {}
    ~Grain() {}

    void setADSR(double sampleRate, float attackSecs, float decaySecs,
        float sustSecs, float relSecs)
    {
        adsr.setSampleRate(sampleRate);
        
        juce::ADSR::Parameters param(attackSecs, decaySecs, sustSecs, relSecs);
        adsr.setParameters(param);

        adsr.applyEnvelopeToBuffer(grainBuffer, startPos, endPos);
    }

    void setADSR(double sampleRate, juce::int64 attackSamp, juce::int64 decaySamp,
        juce::int64 sustSamp, juce::int64 relSamp)
    {
        adsr.setSampleRate(sampleRate);

        juce::ADSR::Parameters param(attackSamp / sampleRate, decaySamp / sampleRate,
            sustSamp / sampleRate, relSamp / sampleRate);
        adsr.setParameters(param);

        adsr.applyEnvelopeToBuffer(grainBuffer, startPos, endPos);
    }
    
    void cutAlreadyPlayedSamples(juce::int64 currentSample, juce::int64 playedSamples)
    {

    }

    void setBuffer(const juce::AudioBuffer<float>& buff)
    {
        grainBuffer = buff;
    }

    juce::AudioBuffer<float>& getBuffer()
    {
        return grainBuffer;
    }

private:
    juce::AudioBuffer<float> grainBuffer;
    juce::int64 startingSample;

    juce::ADSR adsr;

    // random variation of Grain
    juce::int64 startPos;
    juce::int64 endPos;
    // TODO: ????? lo hago?
    float paning; // -1.f left, 0 centered, 1.f right
    float pitch; // altered pitch of the Grain
};

class GrainSampler
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
    GrainSampler() {}
    ~GrainSampler() {}

    void getNextAudioBlock(juce::AudioBuffer<float>& buffer)
    {
        // usar el sistema de estado del player aqui tambien
        if (currentSample > totalNumSamples)
            return;

        currentSample += buffer.getNumSamples();
    }

private:

    float totalSecsPlayingTime;
    juce::int64 currentSample;
    juce::int64 totalNumSamples;

    float grainDensity;

    // unaltered grains which will be playing
    std::list<Grain> playingGrain;
};

//==============================================================================
/**
*/
class GranularSamplerAudioProcessor  : public juce::AudioProcessor, 
                                       public juce::ChangeListener
{
public:
    //==============================================================================
    GranularSamplerAudioProcessor();
    ~GranularSamplerAudioProcessor() override;

    //==============================================================================
    // main 1 = start();
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    // main 2 = update();
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================

    //==============================================================================
    //================================== My new code ===============================
    //==============================================================================


    // ================================= Parameters ================================
    static juce::AudioProcessorValueTreeState::ParameterLayout
        createParameterLayout();

    // coordinate GUI with real params
    juce::AudioProcessorValueTreeState apvts { *this,
        nullptr, "Parameters", createParameterLayout() };

    using BlockType = juce::AudioBuffer<float>;
    SingleChannelSampleFifo<BlockType> leftChannelFifo{ Channel::Left };
    SingleChannelSampleFifo<BlockType> rightChannelFifo{ Channel::Right };
    
    // =============================================================================

private:
    MonoChain leftChain, rightChain;

    //==============================================================================

#pragma region SamplerPlayer
public:
    bool createReaderFor(juce::File f);

    void changeState(const TransportState newState);

    void updateState(const TransportState newState);

    TransportState getTransportState();
private:
    //TransportState transpState;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::AudioSourcePlayer playerAudioSource;

    FileBufferPlayer buffPlay;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

#pragma endregion SamplerPlayer

    // TODO:
    // buffer comodo con todo el audio (quizas tenga que rehacer el Sampler) -> DONE
    // 
    //==============================================================================

#pragma region GranularSampler

    // grain Duration

    // Density of Grain
    // Number of Grains
    // Duration of reproduction

    // tipo de envolvente (de base solo OSC, pero puedo añadir ADR tambien y existe Gaussinana)

    // posibilidad de que tenga variacion aleatoria?:
    //      duration
    //      pan
    //      pitch
    //      

#pragma endregion GranularSampler

    //==============================================================================

#pragma region EQMethods

    void updatePeakFilter(const ChainSettings& chainSettings);

    void updateLowCutFilters(const ChainSettings& chainSettings);
    void updateHighCutFilters(const ChainSettings& chainSettings);

    void updateFilters();

#pragma endregion EQMethods

    //==============================================================================

    // for debugging
    juce::dsp::Oscillator<float> osc;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GranularSamplerAudioProcessor)
};