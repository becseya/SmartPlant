#include "ScalarAggregator.hpp"

#include <limits>

#define FLOAT_MAX std::numeric_limits<float>::max();
#define FLOAT_MIN std::numeric_limits<float>::min();

// --------------------------------------------------------------------------------------------------------------------

using namespace SmartPlant::Aggregation;

ScalarAggregator::ScalarAggregator(const char* name, bool printAverage, float limitLow, float limitHigh)
    : Aggregator(name)
    , printAverage(printAverage)
    , limitLow(limitLow)
    , limitHigh(limitHigh)
{
    reset();
}

void ScalarAggregator::addSample(float sample)
{
    lastSample = sample;
    sum += sample;
    samples++;

    if (sample < min)
        min = sample;
    if (sample > max)
        max = sample;

    outsideLimit = ((sample < limitLow) || (sample > limitHigh));
}

float ScalarAggregator::getLastSample()
{
    return lastSample;
}

void ScalarAggregator::reset()
{
    sum     = 0;
    samples = 0;
    min     = FLOAT_MAX;
    max     = FLOAT_MIN;
}

void ScalarAggregator::printResult()
{
    if (printAverage)
        LOG_AGGREGATOR("Min: %.2f, Max: %.2f, Avg.: %.2f", min, max, sum / samples)
    else
        LOG_AGGREGATOR("Min: %.2f, Max: %.2f", min, max)
}
