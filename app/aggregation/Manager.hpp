#pragma once

#include "Aggregator.hpp"

#include <array>
#include <cstdio>

namespace SmartPlant {
namespace Aggregation {

template<size_t N>
class Manager
{
  public:
    Manager(const std::array<Aggregator*, N>& aggregators)
        : aggregators(aggregators)
    {}

    void update()
    {
        LOG("-------------- AGGREGATOR DATA START -----------------")
        for (auto& a : aggregators) {
            a->printResult();
        }
        LOG("-------------- AGGREGATOR DATA END -----------------")
    }

  protected:
    const std::array<Aggregator*, N>& aggregators;
};

} // namespace Aggregation
} // namespace SmartPlant
