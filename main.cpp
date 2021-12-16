/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "aggregation/Manager.hpp"
#include "app/RGBLed.hpp"
#include "app/lora/lora.hpp"
#include "app/mode_selector.hpp"
#include "app/sensors/Brightness.hpp"
#include "app/sensors/Gps.hpp"
#include "app/sensors/MMA8451Q.hpp"
#include "app/sensors/Si7021.hpp"
#include "app/sensors/SoilMoisture.hpp"
#include "app/sensors/TCS3472_I2C.hpp"
#include "app/utils/BinaryBuilder.hpp"
#include "app/utils/array.hpp"
#include "app/utils/log.hpp"
#include "app/utils/misc.hpp"
#include "mbed.h"

// minimal for LoRa: 10 + app events: 5
#define MAX_NUMBER_OF_GLOBAL_EVENTS 15

using namespace SmartPlant;
using GlobalEventQueue  = SmartPlant::EventQueue;
using SensorDataBuilder = BinaryBuilder<32>;

// fnction definitions
void buildSensorData(SensorDataBuilder& builder);

// global event queue
GlobalEventQueue globalEvents(MAX_NUMBER_OF_GLOBAL_EVENTS);

// Hardware elements and other classes
I2C          i2cBus(PB_9, PB_8);
BusOut       modeLeds(LED1, LED2);
ModeSelector modeSelector(globalEvents, PB_2, modeLeds);
RGBLed       rgbLed(PB_12, PA_12, PA_11);

// Sensors
Sensors::MMA8451Q     sAccelerometer(i2cBus, PA_14, globalEvents);
Sensors::Brightness   sBrightness(PA_4);
Sensors::SoilMoisture sSoilMoisture(PA_0);
Sensors::Gps          sGps(PA_9, PA_10);
Sensors::Si7021       sTempHum(i2cBus);
Sensors::TCS3472_I2C  sColor(i2cBus);

const auto sensors = make_array<Sensor*>( //
    &sAccelerometer,
    &sBrightness,
    &sSoilMoisture,
    &sGps,
    &sTempHum,
    &sColor);

// Aggregation
const auto aggregators = make_array<Aggregation::Aggregator*>( //
    &sAccelerometer.aggregatorX,
    &sAccelerometer.aggregatorY,
    &sAccelerometer.aggregatorZ,
    &sBrightness.aggregator,
    &sSoilMoisture.aggregator,
    &sTempHum.aggregatorTemp,
    &sTempHum.aggregatorHumidity,
    &sColor.aggregator);

Aggregation::Manager<array_size(aggregators)> aggregationManager(aggregators);
SensorDataBuilder                             sensorDataBuilder;

// --------------------------------------------------------------------------------------------------------------------

void buildSensorData(SensorDataBuilder& builder)
{
    auto gps = sGps.getLastMeasurement();

    builder.append<int16_t>(sAccelerometer.aggregatorX.getLastSample() * 1000); // Accelerometer - x
    builder.append<int16_t>(sAccelerometer.aggregatorY.getLastSample() * 1000); // Accelerometer - y
    builder.append<int16_t>(sAccelerometer.aggregatorZ.getLastSample() * 1000); // Accelerometer - z
    builder.append<uint16_t>(sAccelerometer.getPositionChanges());              // Accelerometer - flips
    builder.append<uint16_t>(sAccelerometer.getTapCount());                     // Accelerometer - taps
    builder.append<int16_t>(sBrightness.aggregator.getLastSample() * 100);      // Brightness
    builder.append<int16_t>(sSoilMoisture.aggregator.getLastSample() * 100);    // Soil moisture

    if (gps.valid) {
        builder.append<float>(gps.lat); // Gps - lat
        builder.append<float>(gps.lon); // Gps - lon
    } else {
        builder.append<float>(0); // Gps - lat
        builder.append<float>(0); // Gps - lon
    }

    builder.append<int16_t>(sTempHum.aggregatorTemp.getLastSample() * 100);     // Temp
    builder.append<int16_t>(sTempHum.aggregatorHumidity.getLastSample() * 100); // Hum
    builder.append<char>(colorToString(sColor.aggregator.getLastSample())[0]);  // Color
}

// -------------------------------------------------- START OF MAIN ---------------------------------------------------

int main()
{
    // callbacks
    Lora::onReceive([](uint8_t port, const uint8_t* buffer, size_t size) -> void {
        LOG_DEBUG_BUFFER(buffer, size, " RX Data on port %u: ", port);
    });

    Lora::setMessageBuilder([&](uint8_t* buffer, size_t size) -> int16_t {
        LOG_DEBUG_BUFFER(sensorDataBuilder.getPtr(), sensorDataBuilder.getSize(), "TX Binary data: ");
        return sensorDataBuilder.copyTo(buffer, size);
    });

    modeSelector.onTick([&](Mode currentMode) -> void {
        LOG("") // extra new line
        aggregationManager.update(currentMode == Mode::Normal);
        for (auto& s : sensors) {
            s->update();
        }

        sensorDataBuilder.reset();
        buildSensorData(sensorDataBuilder);
    });

    // init
    LOG_DEBUG("Initializing LoRa stack...")
    Lora::init(globalEvents);

    LOG_DEBUG("Initializing %u sensors...", sensors.size())
    for (auto& s : sensors) {
        if (!s->init())
            LOG("Failed to initialize %s!", s->getName())
    }

    // infinite loop
    globalEvents.dispatch_forever();
}
