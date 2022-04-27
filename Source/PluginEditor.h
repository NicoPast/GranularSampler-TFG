/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CommonEditor.h"
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

    //==============================================================================

#pragma region Player    
public:
    void updatePlayerState(TransportState state);

private:
    TransportState prevoiusPlayerState;
    juce::TextButton openFileButton, playPlayerButton, stopPlayerButton;

    std::unique_ptr<juce::FileChooser> chooser;

    void openButtonClicked();

    void playPlayerButtonClicked();

    void stopPlayerButtonClicked();

#pragma endregion Player

    //==============================================================================

#pragma region GranularSampler    
public:
    void updateSamplerState(TransportState state);

private:
    TransportState prevoiusSamplerState;
    juce::TextButton playSamplerButton, stopSamplerButton;

    void playSamplerButtonClicked();

    void stopSamplerButtonClicked();

#pragma endregion GranularSampler

    //==============================================================================

#pragma region EQComponents
    void eqResized(juce::Rectangle<int> bounds);

    void eqSetUp(juce::Component::SafePointer<GranularSamplerAudioProcessorEditor> safePtr);

    RotarySliderWithLabels peakFreqSlider,
        peakGainSlider,
        peakQualitySlider,
        lowCutFreqSlider,
        highCutFreqSlider,
        lowCutSlopeSlider,
        highCutSlopeSlider;

    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    Attachment peakFreqSliderAttachment,
        peakGainSliderAttachment,
        peakQualitySliderAttachment,
        lowCutFreqSliderAttachment,
        highCutFreqSliderAttachment,
        lowCutSlopeSliderAttachment,
        highCutSlopeSliderAttachment;

    //================================== BypassedButts ===============================

    PowerButton eqEnabledButton, lowCutBypassedButton, peakBypassedButton, highCutBypassedButton;
    AnalyzerButton analyzerEnabledButton;

    using ButtonAttachment = APVTS::ButtonAttachment;

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
