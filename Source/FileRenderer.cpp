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
    auto bottom = renderFileArea.getBottom();
    auto width = renderFileArea.getWidth();
    auto height = renderFileArea.getHeight();

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.drawImage(background, getLocalBounds().toFloat());

    juce::Path fileRendererPath;
    
    fileRendererPath.startNewSubPath(left, bottom - height * 0.5f);

    if (yVals.size() > 0)
    {
        DBG("===========");
        DBG(juce::findMaximum<float>(&yVals[0], yVals.size()));
        DBG(juce::findMinimum<float>(&yVals[0], yVals.size()));
    }

    int pos = 0;
    for (size_t i = 0; i < yVals.size(); )
    {
        //fileRendererPath.lineTo(left + i, bottom - height * juce::Random::getSystemRandom().nextFloat());
        for (size_t j = 0; j < resolution; ++j)
        {
            //int y = bottom - height * 0.5f + height * yVals[i++];
            fileRendererPath.lineTo(left + pos, bottom - height * 0.5f + yVals[i++] * (height * 1.f));
        }
        pos++;
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
    r.setSize(labelsWidth + 7, labelHeight);
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

void FileRenderer::updateFile(const juce::AudioBuffer<float>& fileBuffer)
{
    DBG("Updating file renderer");
    auto renderArea = getFileRendArea();
    
    int width = renderArea.getWidth() * resolution;
    int size = fileBuffer.getNumSamples();
    yVals.resize(width);

    auto step = 1;
    if (size > width)
        step = size / width;
        
    for (int i = 0; i < width; ++i)
    {
        //auto t = (*(reader + (i * step)));
        //auto t2 = fileBuffer.getSample(0, i * step);
        //yVals[i] = t2;
        // turns into mono
        for (int ch = 0; ch < fileBuffer.getNumChannels(); ++ch)
        {
            yVals[i] = fileBuffer.getSample(ch, i * step) / fileBuffer.getNumChannels();
        }
    }
    //for (size_t i = 0; i < yVals.size(); ++i)
    //{
    //    juce::jmap<float>(yVals[i], -1.f, 1.f, renderArea.getY(), renderArea.getBottom());
    //}
    DBG("===========");
    DBG(juce::findMaximum<float>(&yVals[0], width));
    DBG(juce::findMinimum<float>(&yVals[0], width));

    repaint();
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