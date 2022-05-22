#include "StateManager.h"

#include <Arduino.h>
#include <cmath>

#define TAPWATER_LOWER_LIMIT 2000 // values under this are ignored
#define TAPWATER_RUNNING_UPPER 5500 // water can be considered running under this limit


#define FILTER_CAPSENSE_VALUES_UNDER 2500
#define FILTER_CAPSENSE_VALUES_OVER  8500

#define SOIL_MOISTURE_TOO_DRY  6300
#define SOIL_MOISTURE_DRY      5900
#define SOIL_MOISTURE_OK       5500
#define SOIL_MOISTURE_WET      5100
#define SOIL_MOISTURE_TOO_WET  4700


StateManager::StateManager() 
: m_isDoorOpen(false), m_isWindowOpen(false), m_configuredTemperature(0.0), m_temperature(0.0), m_lightIntensity(LIGHTINTENSITY::AVERAGE) {
}

bool StateManager::isDoorOpen() const {
    return this->m_isDoorOpen;
}

bool StateManager::isWindowOpen() const {
    return this->m_isWindowOpen;
}

int StateManager::getConfiguredTemperatue() const {
    return this->m_configuredTemperature;
}

float StateManager::getTrueTemperature() const {
    return this->m_temperature;
}

bool StateManager::isWaterRunning() const {
    if (this->m_tapWaterCapSenseVal < TAPWATER_LOWER_LIMIT) return false;
    return this->m_tapWaterCapSenseVal < TAPWATER_RUNNING_UPPER;
}

LIGHTINTENSITY StateManager::getLight() const {
    return this->m_lightIntensity;
}

TEMP_STATE StateManager::getTempState() const {
    float diff = std::abs(((float) this->m_configuredTemperature) - this->m_temperature);

    if (diff < 2.0) {
        // 2 degree diff less or more is OK
        return TEMP_STATE::OK;
    }

    if (diff < 4.0) {
        // 4 degree difference is concerning
        return TEMP_STATE::SLIGHT_DIFFERENCE;
    }

    return TEMP_STATE::MAJOR_DIFFERENCE; // very off
}

MOISTURE_LEVEL StateManager::getSoilMoistureLevel() const {
    if (this->m_soilCapSenseVal == 0) MOISTURE_LEVEL::OK; // not initialised yet

    if (this->m_soilCapSenseVal >= SOIL_MOISTURE_TOO_DRY)
        return MOISTURE_LEVEL::TOO_DRY;
    
    if (this->m_soilCapSenseVal <= SOIL_MOISTURE_TOO_WET)
        return MOISTURE_LEVEL::TOO_WET;

    if (this->m_soilCapSenseVal <= SOIL_MOISTURE_WET)
        return MOISTURE_LEVEL::WET;
    
    if (this->m_soilCapSenseVal >= SOIL_MOISTURE_DRY)
        return MOISTURE_LEVEL::DRY;

    return MOISTURE_LEVEL::OK;
}

StateManager* StateManager::setDoorOpen(bool state) {
    this->m_isDoorOpen = state;
    return this;
}

StateManager* StateManager::setWindowOpen(bool state) {
    this->m_isWindowOpen = state;
    return this;
}

StateManager* StateManager::setConfiguredTemperature(int temperature) {
    this->m_configuredTemperature = temperature;
    return this;
}


StateManager* StateManager::setLightRaw(int lightVal, int lightMax) {
    int target = (lightVal / (float) lightMax) * 6.0;
    this->m_lightIntensity = static_cast<LIGHTINTENSITY>(target);
    return this;
}

StateManager* StateManager::setTemperature(float temp) {
    this->m_temperature = temp;
    return this;
}

StateManager* StateManager::updateCapsenseWaterTap(uint16_t measurementVal) {
    if (measurementVal <= FILTER_CAPSENSE_VALUES_UNDER || measurementVal >= FILTER_CAPSENSE_VALUES_OVER) return this;
    this->m_tapWaterCapSenseVal = measurementVal;
    return this;
}

StateManager* StateManager::updateCapsenseSoilMoisture(uint16_t measurementVal) {
    if (measurementVal <= FILTER_CAPSENSE_VALUES_UNDER || measurementVal >= FILTER_CAPSENSE_VALUES_OVER) return this;
    this->m_soilCapSenseVal = measurementVal;
    return this;
}
