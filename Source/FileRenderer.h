/*
  ==============================================================================

    FileRenderer.h
    Created: 29 Jul 2022 10:23:29pm
    Author:  aGoat

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class FileRenderer  : public juce::Component
{
public:
    FileRenderer(GranularSamplerAudioProcessor& p);
    ~FileRenderer() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void updateFile(const juce::AudioBuffer<float>& fileBuffer);

private:
    GranularSamplerAudioProcessor& audioProcessor;
    juce::Image background;

    std::vector<float> yVals;

    size_t resolution = 10;

    juce::Rectangle<int> getRenderArea();

    juce::Rectangle<int> getFileRendArea();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileRenderer)
};
