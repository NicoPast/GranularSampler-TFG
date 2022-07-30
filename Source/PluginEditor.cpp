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
    fileRenderer(p),
    granularSamplerGainSlider(*audioProcessor.apvts.getParameter("GranularSampler Gain"), "dB"),
    grainDensitySlider(*audioProcessor.apvts.getParameter("Grain Density"), "%"),
    grainMinLengthSlider(*audioProcessor.apvts.getParameter("Grain Min Length"), "sec"),
    grainMaxLengthSlider(*audioProcessor.apvts.getParameter("Grain Max Length"), "sec"),
    grainMinStartPosSlider(*audioProcessor.apvts.getParameter("Grain Min StartPos"), "%"),
    grainMaxStartPosSlider(*audioProcessor.apvts.getParameter("Grain Max StartPos"), "%"),
    
    grainEnvelopeTypeSlider(*audioProcessor.apvts.getParameter("Grain Envelope Type"), ""),
    grainADSRAttackSlider(*audioProcessor.apvts.getParameter("Grain ADSR Attack"), "%"),
    grainADSRDecaySlider(*audioProcessor.apvts.getParameter("Grain ADSR Decay"), "%"),
    grainADSRSustainSlider(*audioProcessor.apvts.getParameter("Grain ADSR Sustain"), "%"),
    grainLinLeftSlider(*audioProcessor.apvts.getParameter("Grain Lin Left"), "%"),
    grainLinRightSlider(*audioProcessor.apvts.getParameter("Grain Lin Right"), "%"),
    grainSinLeftSlider(*audioProcessor.apvts.getParameter("Grain Sin Left"), "%"),
    grainSinRightSlider(*audioProcessor.apvts.getParameter("Grain Sin Right"), "%"),

    peakFreqSlider(*audioProcessor.apvts.getParameter("Peak Freq"), "Hz"),
    peakGainSlider(*audioProcessor.apvts.getParameter("Peak Gain"), "dB"),
    peakQualitySlider(*audioProcessor.apvts.getParameter("Peak Quality"), ""),
    lowCutFreqSlider(*audioProcessor.apvts.getParameter("LowCut Freq"), "Hz"),
    lowCutSlopeSlider(*audioProcessor.apvts.getParameter("LowCut Slope"), "db/Oct"),
    highCutFreqSlider(*audioProcessor.apvts.getParameter("HighCut Freq"), "Hz"),
    highCutSlopeSlider(*audioProcessor.apvts.getParameter("HighCut Slope"), "db/Oct"),

    granularSamplerGainSliderAttachment(audioProcessor.apvts, "GranularSampler Gain", granularSamplerGainSlider),
    grainDensitySliderAttachment(audioProcessor.apvts, "Grain Density", grainDensitySlider),
    grainMinLenghtSliderAttachment(audioProcessor.apvts, "Grain Min Length", grainMinLengthSlider),
    grainMaxLenghtSliderAttachment(audioProcessor.apvts, "Grain Max Length", grainMaxLengthSlider),
    grainMinStartPosSliderAttachment(audioProcessor.apvts, "Grain Min StartPos", grainMinStartPosSlider),
    grainMaxStartPosSliderAttachment(audioProcessor.apvts, "Grain Max StartPos", grainMaxStartPosSlider),
    
    grainEnvelopeTypeSliderAttachment(audioProcessor.apvts, "Grain Envelope Type", grainEnvelopeTypeSlider),
    grainADSRAttackSliderAttachment(audioProcessor.apvts, "Grain ADSR Attack", grainADSRAttackSlider),
    grainADSRDecaySliderAttachment(audioProcessor.apvts, "Grain ADSR Decay", grainADSRDecaySlider),
    grainADSRSustainSliderAttachment(audioProcessor.apvts, "Grain ADSR Sustain", grainADSRSustainSlider),
    grainLinLeftSliderAttachment(audioProcessor.apvts, "Grain Lin Left", grainLinLeftSlider),
    grainLinRightSliderAttachment(audioProcessor.apvts, "Grain Lin Right", grainLinRightSlider),
    grainSinLeftSliderAttachment(audioProcessor.apvts, "Grain Sin Left", grainSinLeftSlider),
    grainSinRightSliderAttachment(audioProcessor.apvts, "Grain Sin Right", grainSinRightSlider),

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

    granularSamplerGainSlider.labels.add({ 0.f, "0dB" });
    granularSamplerGainSlider.labels.add({ 1.f, "10dB" });

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

    grainADSRAttackSlider.labels.add({ 0.f, "0%" });
    grainADSRAttackSlider.labels.add({ 1.f, "100%" });

    grainADSRDecaySlider.labels.add({ 0.f, "0%" });
    grainADSRDecaySlider.labels.add({ 1.f, "100%" });

    grainADSRSustainSlider.labels.add({ 0.f, "0%" });
    grainADSRSustainSlider.labels.add({ 1.f, "100%" });

    grainLinLeftSlider.labels.add({ 0.f, "0%" });
    grainLinLeftSlider.labels.add({ 1.f, "100%" });

    grainLinRightSlider.labels.add({ 0.f, "0%" });
    grainLinRightSlider.labels.add({ 1.f, "100%" });

    grainSinLeftSlider.labels.add({ 0.f, "1%" });
    grainSinLeftSlider.labels.add({ 1.f, "100%" });

    grainSinRightSlider.labels.add({ 0.f, "0%" });
    grainSinRightSlider.labels.add({ 1.f, "100%" });


    for (auto* comp : getComps()) {
        addAndMakeVisible(comp);
    }

    grainADSRAttackSlider.setVisible(false);
    grainADSRDecaySlider.setVisible(false);
    grainADSRSustainSlider.setVisible(false);
    grainLinLeftSlider.setVisible(false);
    grainLinRightSlider.setVisible(false);
    grainSinLeftSlider.setVisible(false);
    grainSinRightSlider.setVisible(false);

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

    fileRenderer.setLookAndFeel(&lnf);

    updateEnvelopeType(static_cast<EnvelopeType>(audioProcessor.apvts.getRawParameterValue("Grain Envelope Type")->load()));

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

    granularSamplerGainSlider.setLookAndFeel(&lnf);
    grainDensitySlider.setLookAndFeel(&lnf);
    grainMinLengthSlider.setLookAndFeel(&lnf);
    grainMaxLengthSlider.setLookAndFeel(&lnf);
    grainMinStartPosSlider.setLookAndFeel(&lnf);
    grainMaxStartPosSlider.setLookAndFeel(&lnf);

    grainEnvelopeTypeSlider.setLookAndFeel(&lnf);
    grainADSRAttackSlider.setLookAndFeel(&lnf);
    grainADSRDecaySlider.setLookAndFeel(&lnf);
    grainADSRSustainSlider.setLookAndFeel(&lnf);
    grainLinLeftSlider.setLookAndFeel(&lnf);
    grainLinRightSlider.setLookAndFeel(&lnf);
    grainSinLeftSlider.setLookAndFeel(&lnf);
    grainSinRightSlider.setLookAndFeel(&lnf);

    grainDensityLabel.setText("Grain Density", juce::NotificationType::dontSendNotification);
    grainDensityLabel.setFont(juce::Font(12.0f, juce::Font::FontStyleFlags::plain));
    grainDensityLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    grainDensityLabel.setJustificationType(juce::Justification::centred);

    gainLabel.setText("Gain", juce::NotificationType::dontSendNotification);
    gainLabel.setFont(juce::Font(12.0f, juce::Font::FontStyleFlags::plain));
    gainLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    gainLabel.setJustificationType(juce::Justification::centred);

    grainDurationLabel.setText("Grain Duration Range", juce::NotificationType::dontSendNotification);
    grainDurationLabel.setFont(juce::Font(12.0f, juce::Font::FontStyleFlags::plain));
    grainDurationLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    grainDurationLabel.setJustificationType(juce::Justification::centred);

    sourceRangeLabel.setText("Audio Source Range", juce::NotificationType::dontSendNotification);
    sourceRangeLabel.setFont(juce::Font(12.0f, juce::Font::FontStyleFlags::plain));
    sourceRangeLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    sourceRangeLabel.setJustificationType(juce::Justification::centred);

    envelopeLabel.setText("Grain Envelope", juce::NotificationType::dontSendNotification);
    envelopeLabel.setFont(juce::Font(12.0f, juce::Font::FontStyleFlags::plain));
    envelopeLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    envelopeLabel.setJustificationType(juce::Justification::centred);

    creditsLabel.setText("Created by Nicolas Pastore Burgos", juce::NotificationType::dontSendNotification);
    creditsLabel.setFont(juce::Font(10.0f, juce::Font::FontStyleFlags::plain));
    creditsLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    creditsLabel.setJustificationType(juce::Justification::centred);

    eqSetUp(safePtr);

    updateEq();

    setSize (1200, 700);
}

GranularSamplerAudioProcessorEditor::~GranularSamplerAudioProcessorEditor()
{
    openFileButton.setLookAndFeel(nullptr);
    playPlayerButton.setLookAndFeel(nullptr);
    stopPlayerButton.setLookAndFeel(nullptr);
    playSamplerButton.setLookAndFeel(nullptr);
    stopSamplerButton.setLookAndFeel(nullptr);

    fileRenderer.setLookAndFeel(nullptr);

    granularSamplerGainSlider.setLookAndFeel(nullptr);
    grainDensitySlider.setLookAndFeel(nullptr);
    grainMinLengthSlider.setLookAndFeel(nullptr);
    grainMaxLengthSlider.setLookAndFeel(nullptr);
    grainMinStartPosSlider.setLookAndFeel(nullptr);
    grainMaxStartPosSlider.setLookAndFeel(nullptr);

    grainEnvelopeTypeSlider.setLookAndFeel(nullptr);
    grainADSRAttackSlider.setLookAndFeel(nullptr);
    grainADSRDecaySlider.setLookAndFeel(nullptr);
    grainADSRSustainSlider.setLookAndFeel(nullptr);
    grainLinLeftSlider.setLookAndFeel(nullptr);
    grainLinRightSlider.setLookAndFeel(nullptr);
    grainSinLeftSlider.setLookAndFeel(nullptr);
    grainSinRightSlider.setLookAndFeel(nullptr);

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

    // DBG
    //g.setColour(juce::Colours::red);
    //g.drawRect(grainDurationLabel.getBounds());

    //g.setColour(juce::Colours::blue);
    //g.drawRect(grainMinStartPosSlider.getBounds());
}

void GranularSamplerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();

    auto boundsEQ = bounds.removeFromRight(bounds.getWidth() * 0.5f);

    eqResized(boundsEQ);
    //eq.resized(boundsEQ);

    auto fileButtonArea = bounds.removeFromTop(bounds.getHeight() * 0.333f);

    auto fileRendererArea = fileButtonArea.removeFromBottom(fileButtonArea.getHeight() * 0.75f);

    fileRenderer.setBounds(fileRendererArea);

    auto buttonsArea = fileButtonArea.removeFromRight(fileButtonArea.getWidth() * 0.666f);

    openFileButton.setBounds(fileButtonArea);

    auto playerButtonsArea = buttonsArea.removeFromTop(buttonsArea.getHeight() * 0.5f);

    auto stopPlayerButtonArea = playerButtonsArea.removeFromRight(playerButtonsArea.getWidth() * 0.5f);

    playPlayerButton.setBounds(playerButtonsArea);
    stopPlayerButton.setBounds(stopPlayerButtonArea);

    auto stopSamplerButtonArea = buttonsArea.removeFromRight(buttonsArea.getWidth() * 0.5f);

    playSamplerButton.setBounds(buttonsArea);
    stopSamplerButton.setBounds(stopSamplerButtonArea);

    //auto infoBar = bounds.removeFromTop(25);
    auto rightArea = bounds.removeFromRight(bounds.getWidth() * 0.5f);
    
    // left side
    auto densityBoundsSlider = bounds.removeFromTop(bounds.getHeight() * 0.5f);
    grainDensityLabel.setBounds(densityBoundsSlider.removeFromTop(25));
    grainDensitySlider.setBounds(densityBoundsSlider);

    // lower-upper
    auto lengthSliders = bounds.removeFromTop(bounds.getHeight() * 0.5f);
    auto titleDurationArea = lengthSliders.removeFromTop(25);
    grainDurationLabel.setBounds(titleDurationArea);

    auto minLengthSlider = lengthSliders.removeFromLeft(lengthSliders.getWidth() * 0.5f);
    grainMinLengthSlider.setBounds(minLengthSlider);
    grainMaxLengthSlider.setBounds(lengthSliders);

    // lower-lower
    auto titleLengthSliders = bounds.removeFromTop(25);
    sourceRangeLabel.setBounds(titleLengthSliders);
    
    auto minStartPosSlider = bounds.removeFromLeft(bounds.getWidth() * 0.5f);
    grainMinStartPosSlider.setBounds(minStartPosSlider);
    grainMaxStartPosSlider.setBounds(bounds);

    // right side
    auto rightLowArea = rightArea.removeFromBottom(rightArea.getHeight() * 0.5f);
    gainLabel.setBounds(rightArea.removeFromTop(25));
    granularSamplerGainSlider.setBounds(rightArea);

    // lower
    envelopeLabel.setBounds(rightLowArea.removeFromTop(25));

    auto envelopeSlidersArea = rightLowArea.removeFromRight(rightLowArea.getWidth() * 0.333f);
    creditsLabel.setBounds(rightLowArea.removeFromBottom(25));
    rightLowArea.removeFromTop(25);
    grainEnvelopeTypeSlider.setBounds(rightLowArea);

    auto envelopeLinealSlidersArea = envelopeSlidersArea;
    auto envelopeLinealRightArea = envelopeLinealSlidersArea.removeFromBottom(envelopeLinealSlidersArea.getHeight() * 0.5f);

    grainLinLeftSlider.setBounds(envelopeLinealSlidersArea);
    grainLinRightSlider.setBounds(envelopeLinealRightArea);

    grainSinLeftSlider.setBounds(envelopeLinealSlidersArea);
    grainSinRightSlider.setBounds(envelopeLinealRightArea);

    auto adsrDecayArea = envelopeSlidersArea.removeFromBottom(envelopeSlidersArea.getHeight() * 0.666f);
    auto adsrSustainArea = adsrDecayArea.removeFromBottom(adsrDecayArea.getHeight() * 0.5f);
    grainADSRAttackSlider.setBounds(envelopeSlidersArea);
    grainADSRDecaySlider.setBounds(adsrDecayArea);
    grainADSRSustainSlider.setBounds(adsrSustainArea);
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
        | juce::FileBrowserComponent::FileChooserFlags::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)           // [8]
        {
            auto file = fc.getResult();

            if (file != juce::File{})                                                      // [9]
            {
                if (audioProcessor.createReaderFor(file))
                {
                    playPlayerButton.setEnabled(true);                                                      // [13]
                    playSamplerButton.setEnabled(true);
                    fileRenderer.updateFile(audioProcessor.getFileBuffer());
                }
                else
                {
                    playPlayerButton.setEnabled(false);
                    playSamplerButton.setEnabled(false);
                }
                stopPlayerButton.setEnabled(false);
                stopSamplerButton.setEnabled(false);
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

void GranularSamplerAudioProcessorEditor::updateEnvelopeType(EnvelopeType type)
{
    if (envelopeType != type)
    {
        const juce::MessageManagerLock mmLock;
        switch (envelopeType)
        {
        case ADSR:
            grainADSRAttackSlider.setVisible(false);
            grainADSRDecaySlider.setVisible(false);
            grainADSRSustainSlider.setVisible(false);
            break;
        case Lineal:
            grainLinLeftSlider.setVisible(false);
            grainLinRightSlider.setVisible(false);
            break;
        case Sinusoid:
            grainSinLeftSlider.setVisible(false);
            grainSinRightSlider.setVisible(false);
            break;
        default:
            break;
        }

        envelopeType = type;

        switch (envelopeType)
        {
        case ADSR:
            grainADSRAttackSlider.setVisible(true);
            grainADSRDecaySlider.setVisible(true);
            grainADSRSustainSlider.setVisible(true);
            break;
        case Lineal:
            grainLinLeftSlider.setVisible(true);
            grainLinRightSlider.setVisible(true);
            break;
        case Sinusoid:
            grainSinLeftSlider.setVisible(true);
            grainSinRightSlider.setVisible(true);
            break;
        default:
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
    auto analyzerEnabledArea = bounds.removeFromTop(30);

    analyzerEnabledArea.setWidth(bounds.getWidth() * 3.f / 8.f);
    analyzerEnabledArea.setX(bounds.getTopLeft().getX() + 5);
    analyzerEnabledArea.removeFromTop(5);
    auto eqEnableArea = analyzerEnabledArea.removeFromLeft(analyzerEnabledArea.getWidth() * 0.2f);
    auto titleArea = analyzerEnabledArea;

    eqEnabledButton.setBounds(eqEnableArea);
    eqTitleLabel.setBounds(titleArea);

    analyzerEnabledArea.setX(bounds.getTopRight().getX() - analyzerEnabledArea.getWidth() - 5);
    analyzerEnabledButton.setBounds(analyzerEnabledArea);

    // le da un poco de margen
    bounds.removeFromTop(5);

    float hRatio = 25.f / 100.f; //JUCE_LIVE_CONSTANT(33) / 100.0f;
    auto respondArea = bounds.removeFromTop(bounds.getHeight() * hRatio);

    responseCurveComponent.setBounds(respondArea);

    bounds.removeFromTop(5);

    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);

    auto bypassedArea = lowCutArea.removeFromTop(50);
    lowCutBypassedButton.setBounds(bypassedArea.removeFromTop(bypassedArea.getHeight() * 0.5f));
    lowCutLabel.setBounds(bypassedArea);
    lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5));
    lowCutSlopeSlider.setBounds(lowCutArea);

    bypassedArea = highCutArea.removeFromTop(50);
    highCutBypassedButton.setBounds(bypassedArea.removeFromTop(bypassedArea.getHeight() * 0.5f));
    highCutLabel.setBounds(bypassedArea);
    highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5));
    highCutSlopeSlider.setBounds(highCutArea);

    bypassedArea = bounds.removeFromTop(50);
    peakBypassedButton.setBounds(bypassedArea.removeFromTop(bypassedArea.getHeight() * 0.5f));
    bandFilterLabel.setBounds(bypassedArea);
    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33f));
    peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5f));
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
            comp->updateEq();
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

    eqTitleLabel.setText("3 Bands Equalizer", juce::NotificationType::dontSendNotification);
    eqTitleLabel.setFont(juce::Font(16.0f, juce::Font::FontStyleFlags::bold));
    eqTitleLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    eqTitleLabel.setJustificationType(juce::Justification::centredLeft);

    lowCutLabel.setText("LowCut Filter", juce::NotificationType::dontSendNotification);
    lowCutLabel.setFont(juce::Font(12.0f, juce::Font::FontStyleFlags::plain));
    lowCutLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    lowCutLabel.setJustificationType(juce::Justification::centred);

    highCutLabel.setText("HighCut Filter", juce::NotificationType::dontSendNotification);
    highCutLabel.setFont(juce::Font(12.0f, juce::Font::FontStyleFlags::plain));
    highCutLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    highCutLabel.setJustificationType(juce::Justification::centred);

    bandFilterLabel.setText("Band Filter", juce::NotificationType::dontSendNotification);
    bandFilterLabel.setFont(juce::Font(12.0f, juce::Font::FontStyleFlags::plain));
    bandFilterLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    bandFilterLabel.setJustificationType(juce::Justification::centred);
}

void GranularSamplerAudioProcessorEditor::updateEq()
{
    bool bypassed = eqEnabledButton.getToggleState();

    peakBypassedButton.setEnabled(!bypassed);
    auto state = peakBypassedButton.getToggleState();
    peakFreqSlider.setEnabled(!bypassed && !state);
    peakGainSlider.setEnabled(!bypassed && !state);
    peakQualitySlider.setEnabled(!bypassed && !state);

    lowCutBypassedButton.setEnabled(!bypassed);
    state = lowCutBypassedButton.getToggleState();
    lowCutFreqSlider.setEnabled(!bypassed && !state);
    lowCutSlopeSlider.setEnabled(!bypassed && !state);

    highCutBypassedButton.setEnabled(!bypassed);
    state = highCutBypassedButton.getToggleState();
    highCutFreqSlider.setEnabled(!bypassed && !state);
    highCutSlopeSlider.setEnabled(!bypassed && !state);

    analyzerEnabledButton.setEnabled(!bypassed);
    state = analyzerEnabledButton.getToggleState();
    responseCurveComponent.toggleAnalysisEnablement(!bypassed && state);

    eqTitleLabel.setColour(juce::Label::textColourId, bypassed ? juce::Colours::darkgrey : juce::Colours::lightgrey);
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

        &fileRenderer,

        &granularSamplerGainSlider,
        &grainDensitySlider,
        &grainMinLengthSlider,
        &grainMaxLengthSlider,
        &grainMinStartPosSlider,
        &grainMaxStartPosSlider,

        &grainEnvelopeTypeSlider,
        &grainADSRAttackSlider,
        &grainADSRDecaySlider,
        &grainADSRSustainSlider,
        &grainLinLeftSlider,
        &grainLinRightSlider,
        &grainSinLeftSlider,
        &grainSinRightSlider,

        &grainDensityLabel,
        &gainLabel,
        &grainDurationLabel,
        &sourceRangeLabel,
        &envelopeLabel,
        &creditsLabel,

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
        &analyzerEnabledButton,

        &eqTitleLabel,
        &lowCutLabel,
        &highCutLabel,
        &bandFilterLabel
    };
}