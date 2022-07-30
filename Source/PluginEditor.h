/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CommonEditor.h"
#include "FileRenderer.h"

#include "Analyzer.h"

#include "EQEditor.h"

//==============================================================================

/**
*/
class GranularSamplerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    GranularSamplerAudioProcessorEditor (GranularSamplerAudioProcessor& p);
    ~GranularSamplerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    //================================== My new code ===============================
    //==============================================================================


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GranularSamplerAudioProcessor& audioProcessor;

    using APVTS = juce::AudioProcessorValueTreeState;
    using SliderAttachment = APVTS::SliderAttachment;
    using ButtonAttachment = APVTS::ButtonAttachment;

    //==============================================================================

#pragma region Player    
public:
    void updatePlayerState(TransportState state);

private:
    TransportState prevoiusPlayerState;
    juce::TextButton openFileButton, playPlayerButton, stopPlayerButton;

    FileRenderer fileRenderer;

    std::unique_ptr<juce::FileChooser> chooser;

    void openButtonClicked();

    void playPlayerButtonClicked();

    void stopPlayerButtonClicked();

#pragma endregion Player

    //==============================================================================

#pragma region GranularSampler    
public:
    void updateSamplerState(TransportState state);

    void updateEnvelopeType(EnvelopeType type);

private:
    TransportState prevoiusSamplerState;
    juce::TextButton playSamplerButton, stopSamplerButton;

    RotarySliderWithLabels granularSamplerGainSlider, grainDensitySlider,
        grainMinLengthSlider, grainMaxLengthSlider,
        grainMinStartPosSlider, grainMaxStartPosSlider,
        grainEnvelopeTypeSlider,
        grainADSRAttackSlider, grainADSRDecaySlider,
        grainADSRSustainSlider,
        grainLinLeftSlider, grainLinRightSlider,
        grainSinLeftSlider, grainSinRightSlider;

    SliderAttachment granularSamplerGainSliderAttachment, grainDensitySliderAttachment,
        grainMinLenghtSliderAttachment, grainMaxLenghtSliderAttachment,
        grainMinStartPosSliderAttachment, grainMaxStartPosSliderAttachment,
        grainEnvelopeTypeSliderAttachment,
        grainADSRAttackSliderAttachment, grainADSRDecaySliderAttachment,
        grainADSRSustainSliderAttachment,
        grainLinLeftSliderAttachment, grainLinRightSliderAttachment,
        grainSinLeftSliderAttachment, grainSinRightSliderAttachment;

    EnvelopeType envelopeType;

    juce::Label grainDensityLabel, gainLabel,
        grainDurationLabel, sourceRangeLabel, envelopeLabel,
        creditsLabel;

    void playSamplerButtonClicked();

    void stopSamplerButtonClicked();

#pragma endregion GranularSampler

    //==============================================================================

#pragma region EQComponents
    void eqResized(juce::Rectangle<int> bounds);

    void eqSetUp(juce::Component::SafePointer<GranularSamplerAudioProcessorEditor> safePtr);

public:
    void updateEq();

private:
    RotarySliderWithLabels peakFreqSlider,
        peakGainSlider,
        peakQualitySlider,
        lowCutFreqSlider,
        highCutFreqSlider,
        lowCutSlopeSlider,
        highCutSlopeSlider;

    SliderAttachment peakFreqSliderAttachment,
        peakGainSliderAttachment,
        peakQualitySliderAttachment,
        lowCutFreqSliderAttachment,
        highCutFreqSliderAttachment,
        lowCutSlopeSliderAttachment,
        highCutSlopeSliderAttachment;

    juce::Label eqTitleLabel, lowCutLabel, highCutLabel, bandFilterLabel;

    //================================== BypassedButts ===============================

    PowerButton eqEnabledButton, lowCutBypassedButton, peakBypassedButton, highCutBypassedButton;
    AnalyzerButton analyzerEnabledButton;

    ButtonAttachment eqEnabledButtonAttachment,
        lowCutBypassedButtonAttachment,
        peakBypassedButtonAttachment,
        highCutBypassedButtonAttachment,
        analyzerEnabledButtonAttachment;

    //================================== ResponseCurve ===============================

    ResponseCurveComponent responseCurveComponent;

    //EQEditor eq;
#pragma endregion EQComponents

    //==============================================================================

    std::vector<juce::Component*> getComps();

    LookAndFeel lnf;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GranularSamplerAudioProcessorEditor)
};
