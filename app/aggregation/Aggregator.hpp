#pragma once

#include "../utils/log.hpp"

namespace SmartPlant {
namespace Aggregation {

class Aggregator
{
  public:
    Aggregator(const char* name);

    virtual void reset()       = 0;
    virtual void printResult() = 0;

  protected:
    const char* const name;
};

#define LOG_AGGREGATOR(format, ...) LOG("%s: " format, name, __VA_ARGS__)

} // namespace Aggregation
} // namespace SmartPlant
