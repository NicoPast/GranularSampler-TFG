/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum Slope {
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48
};

struct ChainSettings {
    float peakFreq{ 0 }, peakGainInDecibels{ 0 }, peakQuality{ 1.0f };
    float lowCutFreq{ 0 }, highCutFreq{ 0 };
    Slope lowCutSlope{ Slope::Slope_12 }, highCutSlope{ Slope::Slope_12 };
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

//==============================================================================
/**
*/
class SimpleEQAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SimpleEQAudioProcessor();
    ~SimpleEQAudioProcessor() override;

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

    // =============================================================================

private:
    using Filter = juce::dsp::IIR::Filter<float>;

    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;

    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

    MonoChain leftChain, rightChain;

    enum ChainPositions {
        LowCut,
        Peak,
        HighCut
    };

    void updatePeakFilter(const ChainSettings& chainSettings);

    using Coefficients = Filter::CoefficientsPtr;
    static void updateCoefficients(Coefficients& old, const Coefficients& replacements);

    template<typename ChainType, typename CoefficientType>
    void updateCutFilter(ChainType& chainLowCut, const CoefficientType& cutCoefficients, const Slope& lowCutSlope) 
    {
        chainLowCut.template setBypassed<0>(true);
        chainLowCut.template setBypassed<1>(true);
        chainLowCut.template setBypassed<2>(true);
        chainLowCut.template setBypassed<3>(true);

        switch (lowCutSlope) {
        case Slope_12: {
            *chainLowCut.template get<0>().coefficients = *cutCoefficients[0];
            chainLowCut.template setBypassed<0>(false);
            break;
        }
        case Slope_24: {
            *chainLowCut.template get<0>().coefficients = *cutCoefficients[0];
            chainLowCut.template setBypassed<0>(false);
            *chainLowCut.template get<1>().coefficients = *cutCoefficients[1];
            chainLowCut.template setBypassed<1>(false);
            break;
        }
        case Slope_36: {
            *chainLowCut.template get<0>().coefficients = *cutCoefficients[0];
            chainLowCut.template setBypassed<0>(false);
            *chainLowCut.template get<1>().coefficients = *cutCoefficients[1];
            chainLowCut.template setBypassed<1>(false);
            *chainLowCut.template get<2>().coefficients = *cutCoefficients[2];
            chainLowCut.template setBypassed<2>(false);
            break;
        }
        case Slope_48: {
            *chainLowCut.template get<0>().coefficients = *cutCoefficients[0];
            chainLowCut.template setBypassed<0>(false);
            *chainLowCut.template get<1>().coefficients = *cutCoefficients[1];
            chainLowCut.template setBypassed<1>(false);
            *chainLowCut.template get<2>().coefficients = *cutCoefficients[2];
            chainLowCut.template setBypassed<2>(false);
            *chainLowCut.template get<3>().coefficients = *cutCoefficients[3];
            chainLowCut.template setBypassed<3>(false);
            break;
        }
        default:
            break;
        }
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessor)
};
