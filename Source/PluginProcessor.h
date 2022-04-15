/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CommonProcessor.h"
#include "EQProcessor.h"

enum TransportState
{
    Stopped,
    Starting,
    Playing,
    Stopping
};

//==============================================================================
/**
*/
class GranularSamplerAudioProcessor  : public juce::AudioProcessor
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

    bool createReaderFor(juce::File f)
    {
        auto* reader = formatManager.createReaderFor(f);
        if (reader != nullptr)
        {
            auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);   // [11]
            transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
            readerSource.reset(newSource.release());
            return true;
        }
        return false;
    }
private:
    MonoChain leftChain, rightChain;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    TransportState state;

#pragma region EQMethods

    void updatePeakFilter(const ChainSettings& chainSettings);

    void updateLowCutFilters(const ChainSettings& chainSettings);
    void updateHighCutFilters(const ChainSettings& chainSettings);

    void updateFilters();

#pragma endregion EQMethods

    // for debugging
    juce::dsp::Oscillator<float> osc;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GranularSamplerAudioProcessor)
};
