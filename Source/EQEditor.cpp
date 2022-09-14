#include "EQEditor.h"

EQEditor::EQEditor(GranularSamplerAudioProcessor& p) : responseCurveComponent(p),
peakFreqSlider(*p.apvts.getParameter("Peak Freq"), "Hz"),
peakGainSlider(*p.apvts.getParameter("Peak Gain"), "dB"),
peakQualitySlider(*p.apvts.getParameter("Peak Quality"), ""),
lowCutFreqSlider(*p.apvts.getParameter("LowCut Freq"), "Hz"),
lowCutSlopeSlider(*p.apvts.getParameter("LowCut Slope"), "db/Oct"),
highCutFreqSlider(*p.apvts.getParameter("HighCut Freq"), "Hz"),
highCutSlopeSlider(*p.apvts.getParameter("HighCut Slope"), "db/Oct"),

peakFreqSliderAttachment(p.apvts, "Peak Freq", peakFreqSlider),
peakGainSliderAttachment(p.apvts, "Peak Gain", peakGainSlider),
peakQualitySliderAttachment(p.apvts, "Peak Quality", peakQualitySlider),
lowCutFreqSliderAttachment(p.apvts, "LowCut Freq", lowCutFreqSlider),
lowCutSlopeSliderAttachment(p.apvts, "LowCut Slope", lowCutSlopeSlider),
highCutFreqSliderAttachment(p.apvts, "HighCut Freq", highCutFreqSlider),
highCutSlopeSliderAttachment(p.apvts, "HighCut Slope", highCutSlopeSlider),

lowCutBypassedButtonAttachment(p.apvts, "LowCut Bypassed", lowCutBypassedButton),
highCutBypassedButtonAttachment(p.apvts, "HighCut Bypassed", highCutBypassedButton),
peakBypassedButtonAttachment(p.apvts, "Peak Bypassed", peakBypassedButton),
analyzerEnabledButtonAttachment(p.apvts, "Analyzer Bypassed", analyzerEnabledButton)
{
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

    /*peakBypassedButton.setLookAndFeel(&lnf);
    lowCutBypassedButton.setLookAndFeel(&lnf);
    highCutBypassedButton.setLookAndFeel(&lnf);
    analyzerEnabledButton.setLookAndFeel(&lnf);

    auto safePtr = juce::Component::SafePointer<EQEditor>(this);
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
    };*/
}

EQEditor::~EQEditor()
{
    peakBypassedButton.setLookAndFeel(nullptr);
    lowCutBypassedButton.setLookAndFeel(nullptr);
    highCutBypassedButton.setLookAndFeel(nullptr);
    analyzerEnabledButton.setLookAndFeel(nullptr);
}

void EQEditor::init(LookAndFeel& lnf)
{
    //peakBypassedButton.setLookAndFeel(&lnf);
    //lowCutBypassedButton.setLookAndFeel(&lnf);
    //highCutBypassedButton.setLookAndFeel(&lnf);
    //analyzerEnabledButton.setLookAndFeel(&lnf);

    //auto safePtr = juce::Component::SafePointer<EQEditor>(this);
    peakBypassedButton.onClick = [=]()
    {
    //    if (auto* comp = safePtr.getComponent())
    //    {
            auto bypassed = peakBypassedButton.getToggleState();

            peakFreqSlider.setEnabled(!bypassed);
            peakGainSlider.setEnabled(!bypassed);
            peakQualitySlider.setEnabled(!bypassed);
        //}
    };

    lowCutBypassedButton.onClick = [=]()
    {
        //if (auto* comp = safePtr.getComponent())
        //{
            auto bypassed = lowCutBypassedButton.getToggleState();

            lowCutFreqSlider.setEnabled(!bypassed);
            lowCutSlopeSlider.setEnabled(!bypassed);
        //}
    };

    highCutBypassedButton.onClick = [=]()
    {
        //if (auto* comp = safePtr.getComponent())
        //{
            auto bypassed = highCutBypassedButton.getToggleState();

            highCutFreqSlider.setEnabled(!bypassed);
            highCutSlopeSlider.setEnabled(!bypassed);
        //}
    };

    analyzerEnabledButton.onClick = [=]()
    {
        //if (auto* comp = safePtr.getComponent())
        //{
            auto enabled = analyzerEnabledButton.getToggleState();
            responseCurveComponent.toggleAnalysisEnablement(enabled);
        //}
    };
}

void EQEditor::resized(juce::Rectangle<int> bounds)
{
    // 25 pixeles arriba para el botton y titulo
    auto analyzerEnabledArea = bounds.removeFromTop(25);

    analyzerEnabledArea.setWidth(bounds.getWidth() / 4.f);
    analyzerEnabledArea.setX(bounds.getTopLeft().getX() + 5);
    analyzerEnabledArea.removeFromTop(2);

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

std::vector<juce::Component*> EQEditor::getComps()
{
    return
    {
        &peakFreqSlider,
        &peakGainSlider,
        &peakQualitySlider,
        &lowCutFreqSlider,
        &highCutFreqSlider,
        &lowCutSlopeSlider,
        &highCutSlopeSlider,

        &responseCurveComponent,

        &lowCutBypassedButton,
        &peakBypassedButton,
        &highCutBypassedButton,
        &analyzerEnabledButton
    };
}