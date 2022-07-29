/*
  ==============================================================================

    FileRenderer.cpp
    Created: 29 Jul 2022 10:23:29pm
    Author:  aGoat

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FileRenderer.h"

//==============================================================================
FileRenderer::FileRenderer(GranularSamplerAudioProcessor& p) :
    audioProcessor(p)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

FileRenderer::~FileRenderer()
{
}

void FileRenderer::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    //g.fillAll(juce::Colours::black);

    g.drawImage(background, getLocalBounds().toFloat());

    //g.setColour (juce::Colours::grey);
    //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    //g.setColour (juce::Colours::white);
    //g.setFont (14.0f);
    //g.drawText ("FileRenderer", getLocalBounds(),
    //            juce::Justification::centred, true);   // draw some placeholder text
}

void FileRenderer::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    background = juce::Image(juce::Image::PixelFormat::RGB, getWidth(), getHeight(), true);

    juce::Graphics g(background);

    auto renderArea = getRenderArea();
    auto renderFileArea = getFileRendArea();
    auto left = renderFileArea.getX();
    auto right = renderFileArea.getRight();
    auto top = renderFileArea.getY();
    auto bottom = renderFileArea.getBottom();
    auto width = renderFileArea.getWidth();
    auto height = renderFileArea.getHeight();

    //g.setColour(juce::Colours::red);
    //g.drawRect(renderArea);

    g.setColour(juce::Colours::orange);
    g.drawVerticalLine(left, renderArea.getY(), renderArea.getBottom());

    g.setColour(juce::Colour(0u, 172u, 1u));
    g.drawHorizontalLine(top + height * 0.5f, left, right);

    g.setColour(juce::Colours::lightgrey);
    for (float f = 0.f; f < 1.1f; f += 1.f)
    {
        g.drawHorizontalLine(top + height * f, left, right);
    }

    g.setColour(juce::Colours::dimgrey);
    for (float f = 0.25f; f < 1.1f; f += 0.5f)
    {
        g.drawHorizontalLine(top + height * f, left, right);
    }

    g.setColour(juce::Colours::lightgrey);
    juce::Rectangle<int> r;
    r.setSize(renderArea.getWidth() - width, renderArea.getHeight() * 0.2f);
    g.drawFittedText("1.0", r, juce::Justification::centred, 1);
}

juce::Rectangle<int> FileRenderer::getRenderArea()
{
    auto bounds = getLocalBounds();

    //bounds.reduce(JUCE_LIVE_CONSTANT(5), 
    //              JUCE_LIVE_CONSTANT(5));

    bounds.removeFromTop(5);
    bounds.removeFromBottom(15);
    bounds.removeFromLeft(15);
    bounds.removeFromRight(15);

    return bounds;
}

juce::Rectangle<int> FileRenderer::getFileRendArea()
{
    auto bounds = getRenderArea();

    bounds.removeFromTop(10);
    bounds.removeFromBottom(10);
    bounds.removeFromLeft(20);

    return bounds;
}