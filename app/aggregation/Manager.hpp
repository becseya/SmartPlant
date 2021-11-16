#pragma once

#include "Aggregator.hpp"

#include <array>
#include <cstdio>
#include <time.h>

namespace SmartPlant {
namespace Aggregation {

template<size_t N>
class Manager
{
    static constexpr unsigned AGGREGATION_PERIOD_SECONDS = (60 * 60); // every hour

  public:
    Manager(const std::array<Aggregator*, N>& aggregators)
        : aggregators(aggregators)
        , wasActive(false)
        , lastAgregation(0)
    {}

    void update(bool isActive)
    {
        if (isActive && !wasActive) {
            resetAll();
        } else if (isActive && (time(nullptr) >= lastAgregation + AGGREGATION_PERIOD_SECONDS)) {
            LOG("-------------- AGGREGATOR DATA START -----------------")
            for (auto& a : aggregators) {
                a->printResult();
            }
            LOG("-------------- AGGREGATOR DATA END -----------------")

            resetAll();
        }

        wasActive = isActive;
    }

    unsigned getLimitErrorDescription()
    {
        unsigned ret  = 0;
        unsigned mask = (1 << 0);

        for (auto& a : aggregators) {
            if (a->isOutsideLimit()) {
                ret |= mask;
                LOG_DEBUG("Outside limit: %s", a->getName());
            }
            mask <<= 1;
        }

        return ret;
    }

  private:
    void resetAll()
    {
        for (auto& a : aggregators) {
            a->reset();
        }

        lastAgregation = time(nullptr);
    }

    const std::array<Aggregator*, N>& aggregators;
    bool                              wasActive;
    time_t                            lastAgregation;
};

} // namespace Aggregation
} // namespace SmartPlant
