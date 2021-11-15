/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "aggregation/Manager.hpp"
#include "app/mode_selector.hpp"
#include "app/sensors/Brightness.hpp"
#include "app/sensors/Gps.hpp"
#include "app/sensors/MMA8451Q.hpp"
#include "app/sensors/Si7021.hpp"
#include "app/sensors/SoilMoisture.hpp"
#include "app/utils/array.hpp"
#include "app/utils/log.hpp"
#include "mbed.h"

using namespace SmartPlant;

// Hardware elements and other classes
I2C          i2cBus(PB_9, PB_8);
BusOut       modeLeds(LED1, LED2);
ModeSelector modeSelector(PB_2, modeLeds);

// Sensors
Sensors::MMA8451Q     sAccelerometer(i2cBus);
Sensors::Brightness   sBrightness(PA_4);
Sensors::SoilMoisture sSoilMoisture(PA_0);
Sensors::Gps          sGps(PA_9, PA_10);
Sensors::Si7021       sTempHum(i2cBus);

const auto sensors = make_array<Sensor*>( //
    &sAccelerometer,
    &sBrightness,
    &sSoilMoisture,
    &sGps,
    &sTempHum);

// Aggregation
const auto aggregators = make_array<Aggregation::Aggregator*>( //
    &sAccelerometer.aggregatorX,
    &sAccelerometer.aggregatorY,
    &sAccelerometer.aggregatorZ,
    &sBrightness.aggregator,
    &sSoilMoisture.aggregator);

Aggregation::Manager<array_size(aggregators)> aggregationManager(aggregators);

// -------------------------------------------------- START OF MAIN ---------------------------------------------------

int main()
{
    // init
    LOG_DEBUG("Initializing %u sensors...", sensors.size())
    for (auto& s : sensors) {
        if (!s->init())
            LOG("Failed to initialize %s!", s->getName())
    }

    while (true) {
        // update
        modeSelector.update();
        aggregationManager.update(modeSelector.getMode(false) == Mode::Normal);
        for (auto& s : sensors) {
            s->update();
        }

        // sleep
        modeSelector.sleep();
    }
}
