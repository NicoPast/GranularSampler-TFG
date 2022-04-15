#pragma once

#include "JuceHeader.h"
#include "CommonEditor.h"
#include "Analyzer.h"

class EQEditor
{
public:
    EQEditor(GranularSamplerAudioProcessor& p);
    ~EQEditor();

    void init(LookAndFeel& lnf);

	void resized(juce::Rectangle<int> bounds);

    std::vector<juce::Component*> getComps();

    std::vector<juce::Button*> getButtons()
    {
        return
        {
            &lowCutBypassedButton, 
            &peakBypassedButton, 
            &highCutBypassedButton, 
            &analyzerEnabledButton
        };
    }

private:
	bool active;

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

    PowerButton lowCutBypassedButton, peakBypassedButton, highCutBypassedButton;
    AnalyzerButton analyzerEnabledButton;

    using ButtonAttachment = APVTS::ButtonAttachment;

    ButtonAttachment lowCutBypassedButtonAttachment,
        peakBypassedButtonAttachment,
        highCutBypassedButtonAttachment,
        analyzerEnabledButtonAttachment;

    //================================== ResponseCurve ===============================

    ResponseCurveComponent responseCurveComponent;
};

