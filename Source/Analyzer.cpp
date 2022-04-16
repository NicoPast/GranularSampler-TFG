#include "Analyzer.h"

//==============================================================================

void PathProducer::process(juce::Rectangle<float> fftBounds, double sampleRate)
{
    juce::AudioBuffer<float> tempIncomingBuffer;

    while (channelFifo->getNumCompleteBuffersAvailable() > 0)
    {
        if (channelFifo->getAudioBuffer(tempIncomingBuffer))
        {
            auto size = tempIncomingBuffer.getNumSamples();

            // it moves the rest of the data to the front, skipping the first block
            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, 0),
                monoBuffer.getReadPointer(0, size),
                monoBuffer.getNumSamples() - size);

            //writes in the back of the chain the new information
            juce::FloatVectorOperations::copy(
                monoBuffer.getWritePointer(0, monoBuffer.getNumSamples() - size),
                tempIncomingBuffer.getReadPointer(0, 0),
                size);

            channelFFTDataGenerator.produceFFTDataForRendering(monoBuffer, -48.f);
        }
    }

    /*
    if there are FFT data to pull
        if we can pull a buffer
            generate a path
    */
    const auto fftSize = channelFFTDataGenerator.getFFTSize();

    /*
    48000 / 2048 ~= 23Hz <- this is the bin width
    */
    const auto binWidth = sampleRate / (double)fftSize;

    while (channelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0)
    {
        std::vector<float> fftData;
        if (channelFFTDataGenerator.getFFTData(fftData))
        {
            pathProducer.generatePath(fftData, fftBounds, fftSize, binWidth, -48.f);
        }
    }

    /*
    * if we produce faster than we consume
    while there are paths that can be pulled
        pull as many as we can
            display the most recent path
    */

    while (pathProducer.getNumPathsAvailable() > 0)
    {
        pathProducer.getPath(channelFFTPath);
    }
}

//==============================================================================

ResponseCurveComponent::ResponseCurveComponent(GranularSamplerAudioProcessor& p) :
    audioProcessor(p),
    leftPathProducer(audioProcessor.leftChannelFifo),
    rightPathProducer(audioProcessor.rightChannelFifo)

{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->addListener(this);
    }

    updateChain();

    startTimerHz(60);
}

ResponseCurveComponent::~ResponseCurveComponent()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->removeListener(this);
    }
}

void ResponseCurveComponent::parameterValueChanged(int parameterIndex, float newValue)
{
    parametsChanged.set(true);
}

void ResponseCurveComponent::timerCallback()
{
    if (shouldShowFFTAnalysis)
    {
        auto fftBounds = getAnalysisArea().toFloat();
        auto sampleRate = audioProcessor.getSampleRate();

        leftPathProducer.process(fftBounds, sampleRate);
        rightPathProducer.process(fftBounds, sampleRate);
    }

    if (parametsChanged.compareAndSetBool(false, true))
    {
        //couts y se escriben en salida :D
        //DBG("params changed");
        // 
        // update the monochain
        updateChain();
        // signal a repaint
        // we need to repaint all the time because of the analyzer
        //repaint();
    }

    repaint();
}

void ResponseCurveComponent::updateChain()
{
    auto chainSettings = getChainSettings(audioProcessor.apvts);

    eqBypassed = chainSettings.eqBypassed;

    monoChain.setBypassed<ChainPositions::LowCut>(chainSettings.lowCutBypassed);
    monoChain.setBypassed<ChainPositions::Peak>(chainSettings.peakBypassed);
    monoChain.setBypassed<ChainPositions::HighCut>(chainSettings.highCutBypassed);

    auto peakCoefficients = makePeakFilter(chainSettings, audioProcessor.getSampleRate());
    updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);

    auto lowCutCoefficients = makeLowCutFilter(chainSettings, audioProcessor.getSampleRate());
    auto highCutCoefficients = makeHighCutFilter(chainSettings, audioProcessor.getSampleRate());

    updateCutFilter(monoChain.get<ChainPositions::LowCut>(), lowCutCoefficients, chainSettings.lowCutSlope);
    updateCutFilter(monoChain.get<ChainPositions::HighCut>(), highCutCoefficients, chainSettings.highCutSlope);

}

void ResponseCurveComponent::paint(juce::Graphics& g)
{
    using namespace juce;

    g.fillAll(Colours::black);

    g.drawImage(background, getLocalBounds().toFloat());

    auto responseArea = getAnalysisArea();

    auto w = responseArea.getWidth();

    auto& lowCut = monoChain.get<ChainPositions::LowCut>();
    auto& peak = monoChain.get<ChainPositions::Peak>();
    auto& highCut = monoChain.get<ChainPositions::HighCut>();

    auto sampleRate = audioProcessor.getSampleRate();

    std::vector<double> mags;

    mags.resize(w);

    for (int i = 0; i < w; i++) {
        double mag = 1.0;
        auto freq = mapToLog10((double)i / (double)w, 20.0, 20000.0);

        if (!monoChain.isBypassed<ChainPositions::Peak>() && !eqBypassed)
            mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!monoChain.isBypassed<ChainPositions::LowCut>() && !eqBypassed) {
            if (!lowCut.isBypassed<0>())
                mag *= lowCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!lowCut.isBypassed<1>())
                mag *= lowCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!lowCut.isBypassed<2>())
                mag *= lowCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!lowCut.isBypassed<3>())
                mag *= lowCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }

        if (!monoChain.isBypassed<ChainPositions::HighCut>() && !eqBypassed)
        {
            if (!highCut.isBypassed<0>())
                mag *= highCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!highCut.isBypassed<1>())
                mag *= highCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!highCut.isBypassed<2>())
                mag *= highCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!highCut.isBypassed<3>())
                mag *= highCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }

        mags[i] = Decibels::gainToDecibels(mag);
    }

    Path responseCurve;

    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();
    auto map = [outputMin, outputMax](double input)
    {
        // the 24 is because of the peak
        return jmap(input, -24.0, 24.0, outputMin, outputMax);
    };

    responseCurve.startNewSubPath(responseArea.getX(), map(mags.front()));

    for (size_t i = 1; i < mags.size(); i++)
    {
        responseCurve.lineTo(responseArea.getX() + i, map(mags[i]));
    }

    if (shouldShowFFTAnalysis)
    {
        auto leftChannelFFTPath = leftPathProducer.getPath();

        leftChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), responseArea.getY()));

        // paint analyzer
        g.setColour(Colours::skyblue);
        g.strokePath(leftChannelFFTPath, PathStrokeType(1.f));

        auto rightChannelFFTPath = rightPathProducer.getPath();

        rightChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), responseArea.getY()));

        // paint analyzer
        g.setColour(Colours::lightyellow);
        g.strokePath(rightChannelFFTPath, PathStrokeType(1.f));
    }

    g.setColour(Colours::orange);
    g.drawRoundedRectangle(getRenderArea().toFloat(), 4.0f, 1.0f);

    g.setColour(Colours::white);
    g.strokePath(responseCurve, PathStrokeType(2.0f));
}

void ResponseCurveComponent::resized()
{
    using namespace juce;
    background = Image(Image::PixelFormat::RGB, getWidth(), getHeight(), true);

    Graphics g(background);

    Array<float> freqs
    {
        20, 30, 40, 50, 100,
        200, 300, 400, 500, 1000,
        2000, 3000, 4000, 5000, 10000,
        20000
    };

    auto renderArea = getAnalysisArea();
    auto left = renderArea.getX();
    auto right = renderArea.getRight();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();

    Array<float> xs;
    for (auto f : freqs)
    {
        auto normX = mapFromLog10(f, 20.f, 20000.f);
        xs.add(left + width * normX);
    }

    g.setColour(Colours::dimgrey);
    for (auto x : xs)
    {
        g.drawVerticalLine(x, top, bottom);
    }

    Array<float> gain
    {
        -24, -12, 0, 12, 24
    };

    for (auto gDb : gain)
    {
        auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));
        //g.drawHorizontalLine(y, 0, getWidth());
        g.setColour(gDb == 0.f ? Colour(0u, 172u, 1u) : Colours::darkgrey);
        g.drawHorizontalLine(y, left, right);
    }

    g.setColour(Colours::lightgrey);
    const int fontHeight = 10;
    g.setFont(fontHeight);

    for (int i = 0; i < freqs.size(); i++)
    {
        // to draw only the labels i want
        if ((i - 1) % 5 < 2 && i != 0)
            continue;

        auto f = freqs[i];
        auto x = xs[i];

        bool addK = false;
        String str;
        if (f > 999.f) {
            addK = true;
            f /= 1000.f;
        }

        str << f;
        if (addK)
            str << "k";
        str << "Hz";

        auto textWidth = g.getCurrentFont().getStringWidth(str);

        Rectangle<int> r;
        r.setSize(textWidth, fontHeight);
        r.setCentre(x, 0);
        r.setY(1);

        g.drawFittedText(str, r, juce::Justification::centred, 1);
    }

    for (auto gDb : gain)
    {
        auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));

        String str;
        if (gDb > 0)
            str << "+";
        str << gDb;

        auto textWidth = g.getCurrentFont().getStringWidth(str);

        Rectangle<int> r;
        r.setSize(textWidth, fontHeight);
        r.setX(getWidth() - textWidth);
        r.setCentre(r.getCentreX(), y);

        g.setColour(gDb == 0.f ? Colour(0u, 172u, 1u) : Colours::lightgrey);

        g.drawFittedText(str, r, juce::Justification::centred, 1);

        str.clear();
        str << (gDb - 24.f);

        r.setX(1);
        textWidth = g.getCurrentFont().getStringWidth(str);
        r.setSize(textWidth, fontHeight);
        g.setColour(Colours::lightgrey);
        g.drawFittedText(str, r, juce::Justification::centred, 1);
    }
}

juce::Rectangle<int> ResponseCurveComponent::getRenderArea()
{
    auto bounds = getLocalBounds();

    //bounds.reduce(10, //JUCE_LIVE_CONSTANT(5), 
    //              8   //JUCE_LIVE_CONSTANT(5));
    //              );

    bounds.removeFromTop(12);
    bounds.removeFromBottom(2);
    bounds.removeFromLeft(20);
    bounds.removeFromRight(20);

    return bounds;
}

juce::Rectangle<int> ResponseCurveComponent::getAnalysisArea()
{
    auto bounds = getRenderArea();

    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);

    return bounds;
}