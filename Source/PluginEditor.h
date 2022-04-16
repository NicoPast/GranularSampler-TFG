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

    void updateState(TransportState state)
    {
        if (prevoiusState != state)
        {
            prevoiusState = state;
            switch (state)
            {
            case Stopped:                           // [3]
                stopButton.setEnabled(false);
                playButton.setEnabled(true);
                break;

            case Starting:                          // [4]
                playButton.setEnabled(false);
                break;

            case Playing:                           // [5]
                stopButton.setEnabled(true);
                break;

            case Stopping:                          // [6]
                break;

            default:
                DBG("INVALIS STATE RECIEVED");
                break;
            }
        }
    }

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GranularSamplerAudioProcessor& audioProcessor;

    //==============================================================================

    TransportState prevoiusState;
    juce::TextButton openFileButton, playButton, stopButton;

    std::unique_ptr<juce::FileChooser> chooser;

    void openButtonClicked()
    {
        chooser = std::make_unique<juce::FileChooser>("Select a Wave file to play...",
            juce::File{},
            "*.wav");                     // [7]
        auto chooserFlags = juce::FileBrowserComponent::openMode
            | juce::FileBrowserComponent::canSelectFiles;

        chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)           // [8]
            {
                auto file = fc.getResult();

                if (file != juce::File{})                                                      // [9]
                {
                    if (audioProcessor.createReaderFor(file))
                    {
                        playButton.setEnabled(true);                                                      // [13]
                    }
                }
            });
    }

    void playButtonClicked()
    {
        playButton.setEnabled(false);
        audioProcessor.changeState(Starting);
    }

    void stopButtonClicked()
    {
        stopButton.setEnabled(false);
        audioProcessor.changeState(Stopping);
    }

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
