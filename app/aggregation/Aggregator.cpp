#include "Aggregator.hpp"

using namespace SmartPlant::Aggregation;

Aggregator::Aggregator(const char* name)
    : name(name)
    , outsideLimit(false)
{}

bool Aggregator::isOutsideLimit()
{
    return outsideLimit;
}
