/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GranularSamplerAudioProcessor::GranularSamplerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), buffPlay(this)
#endif
{
    formatManager.registerBasicFormats();
    transportSource.addChangeListener(this);
}

GranularSamplerAudioProcessor::~GranularSamplerAudioProcessor()
{
}

//==============================================================================
const juce::String GranularSamplerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GranularSamplerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GranularSamplerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GranularSamplerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GranularSamplerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GranularSamplerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GranularSamplerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GranularSamplerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GranularSamplerAudioProcessor::getProgramName (int index)
{
    return {};
}

void GranularSamplerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GranularSamplerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need.

    transportSource.prepareToPlay(samplesPerBlock, sampleRate);

    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;

    spec.sampleRate = sampleRate;
    
    spec.numChannels = 1;

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    updateFilters();

    leftChannelFifo.prepare(samplesPerBlock);
    rightChannelFifo.prepare(samplesPerBlock);

    osc.initialise([](float x) {return std::sin(x); });

    spec.numChannels = getTotalNumOutputChannels();
    osc.prepare(spec);
    osc.setFrequency(100);
}

void GranularSamplerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GranularSamplerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GranularSamplerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //// This is the place where you'd normally do the guts of your plugin's
    //// audio processing...
    //// Make sure to reset the state if your inner loop is processing
    //// the samples and the outer loop is handling the channels.
    //// Alternatively, you can process the samples with the channels
    //// interleaved by keeping the same state.
    //for (int channel = 0; channel < totalNumInputChannels; ++channel)
    //{
    //    auto* channelData = buffer.getWritePointer (channel);

    //    // ..do something to the data...
    //}

    // =====================================================

    updateFilters();

    // ======================================================

    juce::dsp::AudioBlock<float> block(buffer);
    
    // oscilator debugging
    //buffer.clear();

    //juce::dsp::ProcessContextReplacing<float> steroContext(block);
    //osc.process(steroContext);

    //if (readerSource.get() == nullptr)
    //{
    //    bufferToFill.clearActiveBufferRegion();
    //    return;
    //}

    //juce::AudioSourceChannelInfo bufferToFill(buffer);

    //bufferToFill.numSamples = buffer.getNumSamples();

    //transportSource.getNextAudioBlock(bufferToFill);

    buffPlay.copyNextBlockFromBufferFileTo(buffer);

    //buffer.clear();

    granularSampler.getNextAudioBlock(buffer);

    //buffer.copyFrom(0, 0, *bufferToFill.buffer, 0, bufferToFill.startSample, buffer.getNumSamples());

    //transportSource
    //readerSource->read(&audioBuffer, 0, reader->lengthInSamples, 0, true, true);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);

    // ======================================================

    leftChannelFifo.update(buffer);
    rightChannelFifo.update(buffer);
}

//==============================================================================
bool GranularSamplerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GranularSamplerAudioProcessor::createEditor()
{
    return new GranularSamplerAudioProcessorEditor(*this);
    //codigo modificado por mi
    // we will miss you, default template :(
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void GranularSamplerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void GranularSamplerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        apvts.replaceState(tree);
        updateFilters();
    }
}

bool GranularSamplerAudioProcessor::createReaderFor(juce::File f)
{
    auto* reader = formatManager.createReaderFor(f);
    if (reader != nullptr)
    {
        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);   // [11]
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
        
        // is 2 cause its always 2 from the AudioFormatReaderSource
        juce::AudioBuffer<float> buffer(2, transportSource.getTotalLength());
        juce::AudioSourceChannelInfo bufferToFill(buffer);
        transportSource.start();
        transportSource.getNextAudioBlock(bufferToFill);
        transportSource.stop();
        transportSource.setPosition(0.0);
        buffPlay.setBuffer(buffer);

        granularSampler.setFileBuffer(&buffPlay);

        buffPlay.reset();
        return true;
    }
    return false;
}

void GranularSamplerAudioProcessor::changeState(const TransportState newState)
{
    // ZOMBIE
    //if (transpState != newState)
    //{
        //transpState = newState;

        buffPlay.setState(newState);
        granularSampler.setState(newState);

        //switch (transpState)
        //{
        //case Stopped:                           // [3]
        //    transportSource.setPosition(0.0);
        //    break;

        //case Starting:                          // [4]
        //    transportSource.start();
        //    break;

        //case Playing:
        //    break;

        //case Stopping:                          // [6]
        //    transportSource.stop();
        //    break;
        //default:
        //    DBG("INVALIS STATE RECIEVED");
        //    break;
        //}

        updatePlayerState(newState);
    //}
}

void GranularSamplerAudioProcessor::updatePlayerState(const TransportState newState)
{
    // ZOMBIE
    //transpState = newState;
    if (getActiveEditor() != nullptr)
        static_cast<GranularSamplerAudioProcessorEditor*>(getActiveEditor())->updateState(newState);
}

void GranularSamplerAudioProcessor::updateSamplerState(const TransportState newState)
{
    //if (getActiveEditor() != nullptr)
        //static_cast<GranularSamplerAudioProcessorEditor*>(getActiveEditor())->updateState(newState);
}

TransportState GranularSamplerAudioProcessor::getTransportState()
{
    return buffPlay.getState();
}

void GranularSamplerAudioProcessor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    // ZOMBIE
    //if (source == &transportSource)
    //{
    //    if (transportSource.isPlaying())
    //        changeState(Playing);
    //    else
    //        changeState(Stopped);
    //}
}

void GranularSamplerAudioProcessor::updatePeakFilter(const ChainSettings& chainSettings)
{
    auto peakCoefficients = makePeakFilter(chainSettings, getSampleRate());

    bool byp = chainSettings.peakBypassed || chainSettings.eqBypassed;

    leftChain.setBypassed<ChainPositions::Peak>(byp);
    rightChain.setBypassed<ChainPositions::Peak>(byp);

    updateCoefficients(leftChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
}

void GranularSamplerAudioProcessor::updateLowCutFilters(const ChainSettings& chainSettings)
{
    auto cutCoefficients = makeLowCutFilter(chainSettings, getSampleRate());

    bool byp = chainSettings.lowCutBypassed || chainSettings.eqBypassed;

    leftChain.setBypassed<ChainPositions::LowCut>(byp);
    rightChain.setBypassed<ChainPositions::LowCut>(byp);

    auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();
    auto& rightLowCut = rightChain.get<ChainPositions::LowCut>();

    updateCutFilter(leftLowCut, cutCoefficients, chainSettings.lowCutSlope);
    updateCutFilter(rightLowCut, cutCoefficients, chainSettings.lowCutSlope);
}

void GranularSamplerAudioProcessor::updateHighCutFilters(const ChainSettings& chainSettings)
{
    auto highCutCoefficients = makeHighCutFilter(chainSettings, getSampleRate());

    bool byp = chainSettings.highCutBypassed || chainSettings.eqBypassed;

    leftChain.setBypassed<ChainPositions::HighCut>(byp);
    rightChain.setBypassed<ChainPositions::HighCut>(byp);

    auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();
    auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();

    updateCutFilter(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);
    updateCutFilter(rightHighCut, highCutCoefficients, chainSettings.highCutSlope);
}

void GranularSamplerAudioProcessor::updateFilters()
{
    auto chainSettings = getChainSettings(apvts);
    updateLowCutFilters(chainSettings);
    updateHighCutFilters(chainSettings);
    updatePeakFilter(chainSettings);
}

juce::AudioProcessorValueTreeState::ParameterLayout 
    GranularSamplerAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // name, parameter name, range of parameter, step, skew value, default Val 
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "LowCut Freq", 
        "LowCut Freq", 
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f),
        20.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "HighCut Freq", 
        "HighCut Freq", 
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f),
        20000.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Peak Freq",
        "Peak Freq",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f),
        750.0f));

    // dbs
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Peak Gain",
        "Peak Gain", 
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.5f, 1.0f),
        0.0f));

    // how narrow it is
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Peak Quality", 
        "Peak Quality",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.05f, 1.0f),
        1.0f));

    // choices for the slope types
    juce::StringArray slopeChoices;
    for (int i = 0; i < 4; i++){
        juce::String str;
        str << (12 + i * 12);
        str << " db/Oct";
        slopeChoices.add(str);
    }

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "LowCut Slope",
        "LowCut Slope",
        slopeChoices, 0));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "HighCut Slope",
        "HighCut Slope",
        slopeChoices, 0));

    layout.add(std::make_unique<juce::AudioParameterBool>("EQ Bypassed", "EQ Bypassed", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("LowCut Bypassed", "LowCut Bypassed", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("Peak Bypassed", "Peak Bypassed", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("HighCut Bypassed", "HighCut Bypassed", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("Analyzer Bypassed", "Analyzer Bypassed", true));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GranularSamplerAudioProcessor();
}
