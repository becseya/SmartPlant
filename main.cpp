/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "aggregation/Manager.hpp"
#include "app/RGBLed.hpp"
#include "app/sensors/Brightness.hpp"
#include "app/sensors/Gps.hpp"
#include "app/sensors/MMA8451Q.hpp"
#include "app/sensors/Si7021.hpp"
#include "app/utils/array.hpp"
#include "app/utils/log.hpp"
#include "app/utils/misc.hpp"
#include "mbed.h"

#define MAX_NUMBER_OF_GLOBAL_EVENTS 5
#define SENSOR_UPDATE_PERIOD        2s

using namespace SmartPlant;
using GlobalEventQueue = SmartPlant::EventQueue;

// global event queue
GlobalEventQueue globalEvents(MAX_NUMBER_OF_GLOBAL_EVENTS);

// Hardware elements and other classes
I2C    i2cBus(PB_9, PB_8);
RGBLed rgbLed(PB_12, PA_12, PA_11);

// Sensors
Sensors::Brightness sBrightness(PA_4);
Sensors::Gps        sGps(PA_9, PA_10);
Sensors::Si7021     sTempHum(i2cBus);

const auto sensors = make_array<Sensor*>( //
    &sBrightness,
    &sGps,
    &sTempHum);

// Aggregation
const auto aggregators = make_array<Aggregation::Aggregator*>( //
    &sBrightness.aggregator,
    &sTempHum.aggregatorTemp,
    &sTempHum.aggregatorHumidity);

Aggregation::Manager<array_size(aggregators)> aggregationManager(aggregators);

// -------------------------------------------------- START OF MAIN ---------------------------------------------------

int main()
{
    // init
    LOG_DEBUG("Initializing %u sensors...", sensors.size())
    for (auto& s : sensors) {
        if (!s->init())
            LOG_ERROR("Failed to initialize %s!", s->getName())
    }

    globalEvents.call_every(SENSOR_UPDATE_PERIOD, [&]() -> void {
        for (auto& s : sensors)
            s->update();
    });

    // infinite loop
    globalEvents.dispatch_forever();
}
