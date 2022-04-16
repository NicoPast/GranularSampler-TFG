#pragma once

#include "JuceHeader.h"

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

    bool eqBypassed{ false }, lowCutBypassed{ false }, peakBypassed{ false }, highCutBypassed{ false };
};

using Filter = juce::dsp::IIR::Filter<float>;

using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;

using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

enum ChainPositions {
    LowCut,
    Peak,
    HighCut
};

using Coefficients = Filter::CoefficientsPtr;
void updateCoefficients(Coefficients& old, const Coefficients& replacements);

Coefficients makePeakFilter(const ChainSettings& chainSettings, double sampleRate);

template<int Index, typename ChainType, typename CoefficientType>
void update(ChainType& chain, const CoefficientType& coefficients)
{
    updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]);
    chain.template setBypassed<Index>(false);
}

template<typename ChainType, typename CoefficientType>
void updateCutFilter(ChainType& chain, const CoefficientType& coefficients, const Slope& slope)
{
    chain.template setBypassed<0>(true);
    chain.template setBypassed<1>(true);
    chain.template setBypassed<2>(true);
    chain.template setBypassed<3>(true);

    switch (slope) {
    case Slope_48:
    {
        update<3>(chain, coefficients);
    }
    case Slope_36:
    {
        update<2>(chain, coefficients);
    }
    case Slope_24:
    {
        update<1>(chain, coefficients);
    }
    case Slope_12:
    {
        update<0>(chain, coefficients);
        break;
    }
    default:
        // ERROR -> Incorrect type of Slope given
        DBG("ERROR: Slope type given not supported");
        break;
    }
}

inline auto makeLowCutFilter(const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
        chainSettings.lowCutFreq, sampleRate, 2 * (chainSettings.lowCutSlope + 1));
}

inline auto makeHighCutFilter(const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        chainSettings.highCutFreq, sampleRate, 2 * (chainSettings.highCutSlope + 1));
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);