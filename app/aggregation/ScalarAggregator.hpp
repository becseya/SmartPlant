#pragma once

#include "Aggregator.hpp"

namespace SmartPlant {
namespace Aggregation {

class ScalarAggregator : public Aggregator
{
  public:
    ScalarAggregator(const char* name, bool printAverage, float limitLow, float limitHigh);

    void  addSample(float sample);
    float getLastSample();

    void reset() final;
    void printResult() final;

  protected:
    float       lastSample;
    const bool  printAverage;
    const float limitLow;
    const float limitHigh;

    float sum;
    float min;
    float max;
    int   samples;
};

} // namespace Aggregation
} // namespace SmartPlant
