/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GranularSamplerAudioProcessorEditor::GranularSamplerAudioProcessorEditor(GranularSamplerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
    responseCurveComponent(audioProcessor),
    grainDensitySlider(*audioProcessor.apvts.getParameter("Grain Density"), "%"),
    grainMinLengthSlider(*audioProcessor.apvts.getParameter("Grain Min Length"), "sec"),
    grainMaxLengthSlider(*audioProcessor.apvts.getParameter("Grain Max Length"), "sec"),
    grainMinStartPosSlider(*audioProcessor.apvts.getParameter("Grain Min StartPos"), "%"),
    grainMaxStartPosSlider(*audioProcessor.apvts.getParameter("Grain Max StartPos"), "%"),

    peakFreqSlider(*audioProcessor.apvts.getParameter("Peak Freq"), "Hz"),
    peakGainSlider(*audioProcessor.apvts.getParameter("Peak Gain"), "dB"),
    peakQualitySlider(*audioProcessor.apvts.getParameter("Peak Quality"), ""),
    lowCutFreqSlider(*audioProcessor.apvts.getParameter("LowCut Freq"), "Hz"),
    lowCutSlopeSlider(*audioProcessor.apvts.getParameter("LowCut Slope"), "db/Oct"),
    highCutFreqSlider(*audioProcessor.apvts.getParameter("HighCut Freq"), "Hz"),
    highCutSlopeSlider(*audioProcessor.apvts.getParameter("HighCut Slope"), "db/Oct"),

    grainDensitySliderAttachment(audioProcessor.apvts, "Grain Density", grainDensitySlider),
    grainMinLenghtSliderAttachment(audioProcessor.apvts, "Grain Min Length", grainMinLengthSlider),
    grainMaxLenghtSliderAttachment(audioProcessor.apvts, "Grain Max Length", grainMaxLengthSlider),
    grainMinStartPosSliderAttachment(audioProcessor.apvts, "Grain Min StartPos", grainMinStartPosSlider),
    grainMaxStartPosSliderAttachment(audioProcessor.apvts, "Grain Max StartPos", grainMaxStartPosSlider),
    
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

    grainDensitySlider.labels.add({ 0.f, "0.0%"});
    grainDensitySlider.labels.add({ 1.f, "10.0k%"});

    grainMinLengthSlider.labels.add({ 0.f, "0.01sec" });
    grainMinLengthSlider.labels.add({ 1.f, "300.0sec" });

    grainMaxLengthSlider.labels.add({ 0.f, "0.01sec" });
    grainMaxLengthSlider.labels.add({ 1.f, "300.0sec" });

    grainMinStartPosSlider.labels.add({ 0.f, "0.0%" });
    grainMinStartPosSlider.labels.add({ 1.f, "100.0%" });

    grainMaxStartPosSlider.labels.add({ 0.f, "0.0%" });
    grainMaxStartPosSlider.labels.add({ 1.f, "100.0%" });

    grainDensitySlider.labels.add({ 0.f, "0.0%" });
    grainDensitySlider.labels.add({ 1.f, "10.0k%" });

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

    openFileButton.setLookAndFeel(&lnf);
    openFileButton.setButtonText("Click to OpenFile");

    playPlayerButton.setLookAndFeel(&lnf);
    playPlayerButton.setButtonText("Click to Play File");
    playPlayerButton.setEnabled(false);
    playPlayerButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);

    stopPlayerButton.setLookAndFeel(&lnf);
    stopPlayerButton.setButtonText("Click to Stop File");
    stopPlayerButton.setEnabled(false);
    stopPlayerButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);

    playSamplerButton.setLookAndFeel(&lnf);
    playSamplerButton.setButtonText("Click to Play Sampler");
    playSamplerButton.setEnabled(false);
    playSamplerButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);

    stopSamplerButton.setLookAndFeel(&lnf);
    stopSamplerButton.setButtonText("Click to Stop Sampler");
    stopSamplerButton.setEnabled(false);
    stopSamplerButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);

    auto safePtr = juce::Component::SafePointer<GranularSamplerAudioProcessorEditor>(this);

    //stopButton.onClick = [this] { stopButtonClicked(); };
    openFileButton.onClick = [safePtr]() { 
        if (auto* comp = safePtr.getComponent())
        {
            comp->openButtonClicked();
        }
    };

    playPlayerButton.onClick = [safePtr]() {
        if (auto* comp = safePtr.getComponent())
        {
            comp->playPlayerButtonClicked();
        }
    };
    stopPlayerButton.onClick = [safePtr]() {
        if (auto* comp = safePtr.getComponent())
        {
            comp->stopPlayerButtonClicked();
        }
    };

    playSamplerButton.onClick = [safePtr]() {
        if (auto* comp = safePtr.getComponent())
        {
            comp->playSamplerButtonClicked();
        }
    };
    stopSamplerButton.onClick = [safePtr]() {
        if (auto* comp = safePtr.getComponent())
        {
            comp->stopSamplerButtonClicked();
        }
    };

    prevoiusPlayerState = audioProcessor.getPlayerTransportState();
    prevoiusSamplerState = audioProcessor.getSamplerTransportState();

    eqSetUp(safePtr);

    setSize (1200, 480);
}

GranularSamplerAudioProcessorEditor::~GranularSamplerAudioProcessorEditor()
{
    openFileButton.setLookAndFeel(nullptr);
    playPlayerButton.setLookAndFeel(nullptr);
    stopPlayerButton.setLookAndFeel(nullptr);
    playSamplerButton.setLookAndFeel(nullptr);
    stopSamplerButton.setLookAndFeel(nullptr);

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
}

void GranularSamplerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();

    auto boundsEQ = bounds.removeFromRight(bounds.getWidth() / 2.f);

    eqResized(boundsEQ);
    //eq.resized(boundsEQ);

    auto fileButtonArea = bounds.removeFromTop(bounds.getHeight() / 8.f);

    auto buttonsArea = fileButtonArea.removeFromRight(fileButtonArea.getWidth() * 2.f / 3.f);

    openFileButton.setBounds(fileButtonArea);

    auto playerButtonsArea = buttonsArea.removeFromTop(buttonsArea.getHeight() / 2.f);

    auto stopPlayerButtonArea = playerButtonsArea.removeFromRight(playerButtonsArea.getWidth() / 2.f);

    playPlayerButton.setBounds(playerButtonsArea);
    stopPlayerButton.setBounds(stopPlayerButtonArea);

    auto stopSamplerButtonArea = buttonsArea.removeFromRight(buttonsArea.getWidth() / 2.f);

    playSamplerButton.setBounds(buttonsArea);
    stopSamplerButton.setBounds(stopSamplerButtonArea);

    auto infoBar = bounds.removeFromTop(25);
    bounds.removeFromRight(bounds.getWidth() / 2.f);
    auto densityBoundsSlider = bounds.removeFromTop(bounds.getHeight() / 2.f);
    auto lengthSliders = bounds.removeFromTop(bounds.getHeight() / 2.f);
    auto minLengthSlider = lengthSliders.removeFromLeft(lengthSliders.getWidth() / 2.f);
    
    grainDensitySlider.setBounds(densityBoundsSlider);
    grainMinLengthSlider.setBounds(minLengthSlider);
    grainMaxLengthSlider.setBounds(lengthSliders);

    auto minStartPosSlider = bounds.removeFromLeft(bounds.getWidth() / 2.f);

    grainMinStartPosSlider.setBounds(minStartPosSlider);
    grainMaxStartPosSlider.setBounds(bounds);
}

void GranularSamplerAudioProcessorEditor::updatePlayerState(TransportState state)
{
    if (prevoiusPlayerState != state)
    {
        prevoiusPlayerState = state;

        // locks the thread so it doesn't crash when is called from the processor
        const juce::MessageManagerLock mmLock;
        switch (state)
        {
        case Stopped:                           // [3]
            stopPlayerButton.setEnabled(false);
            playPlayerButton.setEnabled(true);
            break;

        case Starting:                          // [4]
            playPlayerButton.setEnabled(false);
            break;

        case Playing:                           // [5]
            stopPlayerButton.setEnabled(true);
            break;

        case Stopping:                          // [6]
            break;

        default:
            DBG("INVALIS STATE RECIEVED");
            break;
        }
    }
}

void GranularSamplerAudioProcessorEditor::openButtonClicked()
{
    chooser = std::make_unique<juce::FileChooser>("Select a Wave file to play...",
        juce::File{},
        "*.wav");                     // [7]
    auto chooserFlags = juce::FileBrowserComponent::openMode
        | juce::FileBrowserComponent::canSelectFiles;

    playPlayerButton.setEnabled(false);
    stopPlayerButton.setEnabled(false);
    playSamplerButton.setEnabled(false);
    stopSamplerButton.setEnabled(false);

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)           // [8]
        {
            auto file = fc.getResult();

            if (file != juce::File{})                                                      // [9]
            {
                if (audioProcessor.createReaderFor(file))
                {
                    playPlayerButton.setEnabled(true);                                                      // [13]
                    playSamplerButton.setEnabled(true);
                }
            }
        });
}

void GranularSamplerAudioProcessorEditor::playPlayerButtonClicked()
{
    playPlayerButton.setEnabled(false);
    audioProcessor.changePlayerState(Starting);
    audioProcessor.changeSamplerState(Stopping);
}

void GranularSamplerAudioProcessorEditor::stopPlayerButtonClicked()
{
    stopPlayerButton.setEnabled(false);
    audioProcessor.changePlayerState(Stopping);
}

void GranularSamplerAudioProcessorEditor::updateSamplerState(TransportState state)
{
    if (prevoiusSamplerState != state)
    {
        prevoiusSamplerState = state;

        // locks the thread so it doesn't crash when is called from the processor
        const juce::MessageManagerLock mmLock;
        switch (state)
        {
        case Stopped:                           // [3]
            stopSamplerButton.setEnabled(false);
            playSamplerButton.setEnabled(true);
            break;

        case Starting:                          // [4]
            playSamplerButton.setEnabled(false);
            break;

        case Playing:                           // [5]
            stopSamplerButton.setEnabled(true);
            break;

        case Stopping:                          // [6]
            break;

        default:
            DBG("INVALIS STATE RECIEVED");
            break;
        }
    }
}

void GranularSamplerAudioProcessorEditor::playSamplerButtonClicked()
{
    playSamplerButton.setEnabled(false);
    audioProcessor.changeSamplerState(Starting);
    audioProcessor.changePlayerState(Stopping);
}

void GranularSamplerAudioProcessorEditor::stopSamplerButtonClicked()
{
    stopSamplerButton.setEnabled(false);
    audioProcessor.changeSamplerState(Stopping);
}

void GranularSamplerAudioProcessorEditor::eqResized(juce::Rectangle<int> bounds)
{
    // 25 pixeles arriba para el botton y titulo
    auto analyzerEnabledArea = bounds.removeFromTop(25);

    analyzerEnabledArea.setWidth(bounds.getWidth() * 3.f / 8.f);
    analyzerEnabledArea.setX(bounds.getTopLeft().getX() + 5);
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

void GranularSamplerAudioProcessorEditor::eqSetUp(juce::Component::SafePointer<GranularSamplerAudioProcessorEditor> safePtr)
{
    //auto test = eq.getButtons();

    //eq.init(lnf);

    // TODO: wat, why you little... me peta si lo hago desde otra clase
    eqEnabledButton.setLookAndFeel(&lnf);
    peakBypassedButton.setLookAndFeel(&lnf);
    lowCutBypassedButton.setLookAndFeel(&lnf);
    highCutBypassedButton.setLookAndFeel(&lnf);
    analyzerEnabledButton.setLookAndFeel(&lnf);

    eqEnabledButton.onClick = [safePtr]()
    {
        if (auto* comp = safePtr.getComponent())
        {
            bool bypassed = comp->eqEnabledButton.getToggleState();

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
        &openFileButton,
        &playPlayerButton,
        &stopPlayerButton,
        &playSamplerButton,
        &stopSamplerButton,

        &grainDensitySlider,
        &grainMinLengthSlider,
        &grainMaxLengthSlider,
        &grainMinStartPosSlider,
        &grainMaxStartPosSlider,

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