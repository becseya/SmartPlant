#include "ColorAggregator.hpp"

// --------------------------------------------------------------------------------------------------------------------

using namespace SmartPlant::Aggregation;
using SmartPlant::Color;

ColorAggregator::ColorAggregator(const char* name, Color allowedColor)
    : Aggregator(name)
    , allowedColor(allowedColor)
{
    reset();
}

void ColorAggregator::addSample(Color sample)
{
    int idx = static_cast<int>(sample);

    sums[idx]++;
    outsideLimit = (sample != allowedColor);
}

void ColorAggregator::reset()
{
    for (int i = 0; i < Color::NUMBER_OF_COLORS; i++)
        sums[i] = 0;
}

void ColorAggregator::printResult()
{
    LOG_AGGREGATOR("Dominant: %s", colorToString(getMostFrequentColor()));
}

Color ColorAggregator::getMostFrequentColor()
{
    int max    = -1;
    int maxIdx = 0;

    for (int i = 0; i < Color::NUMBER_OF_COLORS; i++) {
        if (sums[i] > max) {
            max    = sums[i];
            maxIdx = i;
        }
    }

    return static_cast<Color>(maxIdx);
}
