#include "EventQueue.hpp"

SmartPlant::EventQueue::EventQueue(size_t maxNumberOfEvents)
    : events::EventQueue(maxNumberOfEvents * EVENTS_EVENT_SIZE)
{}
