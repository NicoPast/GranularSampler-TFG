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

    //g.fillAll(juce::Colours::black);
    // 
    //g.setColour (juce::Colours::grey);
    //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    //g.setColour (juce::Colours::white);
    //g.setFont (14.0f);
    //g.drawText ("FileRenderer", getLocalBounds(),
    //            juce::Justification::centred, true);   // draw some placeholder text

    auto renderFileArea = getFileRendArea();
    auto left = renderFileArea.getX();
    auto top = renderFileArea.getY();
    auto width = renderFileArea.getWidth();
    auto height = renderFileArea.getHeight();

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.drawImage(background, getLocalBounds().toFloat());

    juce::Path fileRendererPath;
    
    fileRendererPath.startNewSubPath(left, top + height * 0.5f);

    for (size_t i = 1; i < width; i++)
    {
        fileRendererPath.lineTo(left + i, top + height * juce::Random::getSystemRandom().nextFloat());
    }

    //g.setColour(juce::Colours::orange);
    //g.drawRoundedRectangle(getRenderArea().toFloat(), 4.0f, 1.0f);

    g.setColour(juce::Colours::lightgrey);
    g.strokePath(fileRendererPath, juce::PathStrokeType(2.0f));
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

    juce::Array<std::string> labels
    {
        "+1.0", "+0.5", "0.0", "-0.5", "-1.0"
    };

    juce::Array<juce::Colour> colours
    {
        juce::Colours::lightgrey,
        juce::Colours::dimgrey,
        juce::Colour(0u, 172u, 1u),
        juce::Colours::dimgrey,
        juce::Colours::lightgrey
    };

    //g.setColour(juce::Colours::red);
    //g.drawRect(renderArea);

    int labelsWidth = renderArea.getWidth() - width;
    int labelHeight = renderArea.getHeight() * 0.2f;
    int upOffset = 5;
    juce::Rectangle<int> r;
    r.setSize(labelsWidth - JUCE_LIVE_CONSTANT(-7), labelHeight);
    r.setX(labelsWidth * 0.2f);

    for (int i = 0; i < 5; i++)
    {
        g.setColour(colours[i]);
        g.drawHorizontalLine(top + height * (i * 0.25f), left, right);

        r.setY(upOffset);
        //g.setColour(juce::Colours::red);
        //g.drawRect(r);
        g.setColour(colours[i]);
        g.drawFittedText(labels[i], r, juce::Justification::centred, 1);
        upOffset += labelHeight;
    }

    g.setColour(juce::Colours::orange);
    g.drawVerticalLine(left, renderArea.getY(), renderArea.getBottom());
}

void FileRenderer::updateFile()
{
    DBG("click");


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