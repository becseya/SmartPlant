#pragma once

#include "Aggregator.hpp"

namespace SmartPlant {
namespace Aggregation {

class ScalarAggregator : public Aggregator
{
  public:
    ScalarAggregator(const char* name, bool printAverage);

    void addSample(float sample);

    void reset() final;
    void printResult() final;

  protected:
    const bool printAverage;
    float      sum;
    float      min;
    float      max;
    int        samples;
};

} // namespace Aggregation
} // namespace SmartPlant
