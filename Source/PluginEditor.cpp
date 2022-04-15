/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GranularSamplerAudioProcessorEditor::GranularSamplerAudioProcessorEditor (GranularSamplerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    responseCurveComponent(audioProcessor),
    peakFreqSlider(*audioProcessor.apvts.getParameter("Peak Freq"), "Hz"),
    peakGainSlider(*audioProcessor.apvts.getParameter("Peak Gain"), "dB"),
    peakQualitySlider(*audioProcessor.apvts.getParameter("Peak Quality"), ""),
    lowCutFreqSlider(*audioProcessor.apvts.getParameter("LowCut Freq"), "Hz"),
    lowCutSlopeSlider(*audioProcessor.apvts.getParameter("LowCut Slope"), "db/Oct"),
    highCutFreqSlider(*audioProcessor.apvts.getParameter("HighCut Freq"), "Hz"),
    highCutSlopeSlider(*audioProcessor.apvts.getParameter("HighCut Slope"), "db/Oct"),

    peakFreqSliderAttachment(audioProcessor.apvts, "Peak Freq", peakFreqSlider),
    peakGainSliderAttachment(audioProcessor.apvts, "Peak Gain", peakGainSlider),
    peakQualitySliderAttachment(audioProcessor.apvts, "Peak Quality", peakQualitySlider),
    lowCutFreqSliderAttachment(audioProcessor.apvts, "LowCut Freq", lowCutFreqSlider),
    lowCutSlopeSliderAttachment(audioProcessor.apvts, "LowCut Slope", lowCutSlopeSlider),
    highCutFreqSliderAttachment(audioProcessor.apvts, "HighCut Freq", highCutFreqSlider),
    highCutSlopeSliderAttachment(audioProcessor.apvts, "HighCut Slope", highCutSlopeSlider),

    eqEnabledButtonAttachment(audioProcessor.apvts, "EQ Bypassed", eqEnabledButton),
    lowCutBypassedButtonAttachment(audioProcessor.apvts, "LowCut Bypassed", lowCutBypassedButton),
    highCutBypassedButtonAttachment(audioProcessor.apvts, "HighCut Bypassed", highCutBypassedButton),
    peakBypassedButtonAttachment(audioProcessor.apvts, "Peak Bypassed", peakBypassedButton),
    analyzerEnabledButtonAttachment(audioProcessor.apvts, "Analyzer Bypassed", analyzerEnabledButton)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    peakFreqSlider.labels.add({ 0.f, "20Hz" });
    peakFreqSlider.labels.add({ 1.f, "20kHz" });

    peakGainSlider.labels.add({ 0.f, "-24dB" });
    peakGainSlider.labels.add({ 1.f, "+24dB" });

    peakQualitySlider.labels.add({ 0.f, "0.1" });
    peakQualitySlider.labels.add({ 1.f, "10.0" });

    lowCutFreqSlider.labels.add({ 0.f, "20Hz" });
    lowCutFreqSlider.labels.add({ 1.f, "20kHz" });

    highCutFreqSlider.labels.add({ 0.f, "20Hz" });
    highCutFreqSlider.labels.add({ 1.f, "20kHz" });

    lowCutSlopeSlider.labels.add({ 0.f, "12" });
    lowCutSlopeSlider.labels.add({ 1.f, "48" });

    highCutSlopeSlider.labels.add({ 0.f, "12" });
    highCutSlopeSlider.labels.add({ 1.f, "48" });


    for (auto* comp : getComps()) {
        addAndMakeVisible(comp);
    }

    openFile.setLookAndFeel(&lnf);

    eqSetUp();

    setSize (1200, 480);
}

GranularSamplerAudioProcessorEditor::~GranularSamplerAudioProcessorEditor()
{
    openFile.setLookAndFeel(nullptr);

    eqEnabledButton.setLookAndFeel(nullptr);
    peakBypassedButton.setLookAndFeel(nullptr);
    lowCutBypassedButton.setLookAndFeel(nullptr);
    highCutBypassedButton.setLookAndFeel(nullptr);
    analyzerEnabledButton.setLookAndFeel(nullptr);
}

//==============================================================================
void GranularSamplerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // old code, the one it came with
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
    
    g.fillAll (juce::Colours::black);

    auto bounds = getLocalBounds();

    auto boundsEQ = bounds.removeFromRight(bounds.getWidth() / 2.f);

    g.setColour(juce::Colours::red);
    g.drawRect(bounds);
}

void GranularSamplerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();

    auto boundsEQ = bounds.removeFromRight(bounds.getWidth() / 2.f);

    //eq.resized(boundsEQ);

    openFile.setBounds(bounds);
    openFile.setButtonText("Click to OpenFile");

    eqResized(boundsEQ);
}

void GranularSamplerAudioProcessorEditor::eqResized(juce::Rectangle<int> bounds)
{
    // 25 pixeles arriba para el botton y titulo
    auto analyzerEnabledArea = bounds.removeFromTop(25);

    analyzerEnabledArea.setWidth(bounds.getWidth() * 3.f / 8.f);
    //analyzerEnabledArea.setX(bounds.getTopLeft().getX() + 5);
    analyzerEnabledArea.removeFromTop(2);
    auto eqEnableArea = analyzerEnabledArea.removeFromLeft(analyzerEnabledArea.getWidth() / 4.f);

    eqEnabledButton.setBounds(eqEnableArea);
    analyzerEnabledButton.setBounds(analyzerEnabledArea);

    // le da un poco de margen
    bounds.removeFromTop(5);

    float hRatio = 25.f / 100.f; //JUCE_LIVE_CONSTANT(33) / 100.0f;
    auto respondArea = bounds.removeFromTop(bounds.getHeight() * hRatio);

    responseCurveComponent.setBounds(respondArea);

    bounds.removeFromTop(5);

    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);

    lowCutBypassedButton.setBounds(lowCutArea.removeFromTop(25));
    lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5));
    lowCutSlopeSlider.setBounds(lowCutArea);

    highCutBypassedButton.setBounds(highCutArea.removeFromTop(25));
    highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5));
    highCutSlopeSlider.setBounds(highCutArea);

    peakBypassedButton.setBounds(bounds.removeFromTop(25));
    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
    peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    peakQualitySlider.setBounds(bounds);
}

void GranularSamplerAudioProcessorEditor::eqSetUp()
{
    //auto test = eq.getButtons();

    //eq.init(lnf);

    // TODO: wat, why you little... me peta si lo hago desde otra clase
    eqEnabledButton.setLookAndFeel(&lnf);
    peakBypassedButton.setLookAndFeel(&lnf);
    lowCutBypassedButton.setLookAndFeel(&lnf);
    highCutBypassedButton.setLookAndFeel(&lnf);
    analyzerEnabledButton.setLookAndFeel(&lnf);

    auto safePtr = juce::Component::SafePointer<GranularSamplerAudioProcessorEditor>(this);
    eqEnabledButton.onClick = [safePtr]()
    {
        if (auto* comp = safePtr.getComponent())
        {
            bool bypassed = comp->eqEnabledButton.getToggleState();

            comp->eqActive = bypassed;

            comp->peakBypassedButton.setEnabled(!bypassed);
            auto state = comp->peakBypassedButton.getToggleState();
            comp->peakFreqSlider.setEnabled(!bypassed && !state);
            comp->peakGainSlider.setEnabled(!bypassed && !state);
            comp->peakQualitySlider.setEnabled(!bypassed && !state);

            comp->lowCutBypassedButton.setEnabled(!bypassed);
            state = comp->lowCutBypassedButton.getToggleState();
            comp->lowCutFreqSlider.setEnabled(!bypassed && !state);
            comp->lowCutSlopeSlider.setEnabled(!bypassed && !state);

            comp->highCutBypassedButton.setEnabled(!bypassed);
            state = comp->highCutBypassedButton.getToggleState();
            comp->highCutFreqSlider.setEnabled(!bypassed && !state);
            comp->highCutSlopeSlider.setEnabled(!bypassed && !state);

            comp->analyzerEnabledButton.setEnabled(!bypassed);
            state = comp->analyzerEnabledButton.getToggleState();
            comp->responseCurveComponent.toggleAnalysisEnablement(!bypassed && state);
        }
    };

    peakBypassedButton.onClick = [safePtr]()
    {
        if (auto* comp = safePtr.getComponent())
        {
            auto bypassed = comp->peakBypassedButton.getToggleState();

            comp->peakFreqSlider.setEnabled(!bypassed);
            comp->peakGainSlider.setEnabled(!bypassed);
            comp->peakQualitySlider.setEnabled(!bypassed);
        }
    };

    lowCutBypassedButton.onClick = [safePtr]()
    {
        if (auto* comp = safePtr.getComponent())
        {
            auto bypassed = comp->lowCutBypassedButton.getToggleState();

            comp->lowCutFreqSlider.setEnabled(!bypassed);
            comp->lowCutSlopeSlider.setEnabled(!bypassed);
        }
    };

    highCutBypassedButton.onClick = [safePtr]()
    {
        if (auto* comp = safePtr.getComponent())
        {
            auto bypassed = comp->highCutBypassedButton.getToggleState();

            comp->highCutFreqSlider.setEnabled(!bypassed);
            comp->highCutSlopeSlider.setEnabled(!bypassed);
        }
    };

    analyzerEnabledButton.onClick = [safePtr]()
    {
        if (auto* comp = safePtr.getComponent())
        {
            auto enabled = comp->analyzerEnabledButton.getToggleState();
            comp->responseCurveComponent.toggleAnalysisEnablement(enabled);
        }
    };
}

std::vector<juce::Component*> GranularSamplerAudioProcessorEditor::getComps() 
{
    return 
    {
        &openFile,

        &peakFreqSlider,
        &peakGainSlider,
        &peakQualitySlider,
        &lowCutFreqSlider,
        &highCutFreqSlider,
        &lowCutSlopeSlider,
        &highCutSlopeSlider,

        &responseCurveComponent,

        &eqEnabledButton,
        &lowCutBypassedButton,
        &peakBypassedButton,
        &highCutBypassedButton,
        &analyzerEnabledButton
    };
}