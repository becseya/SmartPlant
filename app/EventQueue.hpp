
#pragma once

#include "mbed-os/events/include/events/EventQueue.h"
#include <cstddef>

namespace SmartPlant {

struct EventQueue : public events::EventQueue
{
    EventQueue(size_t maxNumberOfEvents);
};

} // namespace SmartPlant
