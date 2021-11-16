#pragma once

#include "../Color.hpp"
#include "Aggregator.hpp"

namespace SmartPlant {
namespace Aggregation {

class ColorAggregator : public Aggregator
{
  public:
    ColorAggregator(const char* name);

    void addSample(Color sample);

    void reset() final;
    void printResult() final;

  private:
    Color getMostFrequentColor();

    int sums[Color::NUMBER_OF_COLORS];
};

} // namespace Aggregation
} // namespace SmartPlant
