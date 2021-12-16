#pragma once

#include "../Color.hpp"
#include "Aggregator.hpp"

namespace SmartPlant {
namespace Aggregation {

class ColorAggregator : public Aggregator
{
  public:
    ColorAggregator(const char* name, Color allowedColor);

    void  addSample(Color sample);
    Color getLastSample();

    void reset() final;
    void printResult() final;

  private:
    Color getMostFrequentColor();

    int         sums[Color::NUMBER_OF_COLORS];
    const Color allowedColor;
    Color       lastSample;
};

} // namespace Aggregation
} // namespace SmartPlant
