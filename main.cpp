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
#include "app/sensors/TCS3472_I2C.hpp"
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
    &sSoilMoisture.aggregator,
    &sTempHum.aggregatorTemp,
    &sTempHum.aggregatorHumidity);

Aggregation::Manager<array_size(aggregators)> aggregationManager(aggregators);

// -------------------------------------------------- START OF MAIN ---------------------------------------------------

Sensors::TCS3472_I2C sColor(PB_9, PB_8);

int main()
{
    // init
    LOG_DEBUG("Initializing %u sensors...", sensors.size())
    for (auto& s : sensors) {
        if (!s->init())
            LOG("Failed to initialize %s!", s->getName())
    }

    sColor.enablePowerAndRGBC();
    sColor.setIntegrationTime(100);

    while (true) {
        // update
        LOG("") // extra new line
        modeSelector.update();
        aggregationManager.update(modeSelector.getMode(false) == Mode::Normal);
        for (auto& s : sensors) {
            s->update();
        }

        int rgb_readings[4];
        sColor.getAllColors(rgb_readings);
        LOG("COLOR: red: %d, green: %d, blue: %d, clear: %d",
            rgb_readings[0],
            rgb_readings[1],
            rgb_readings[2],
            rgb_readings[3]);

        // sleep
        modeSelector.sleep();
    }
}
