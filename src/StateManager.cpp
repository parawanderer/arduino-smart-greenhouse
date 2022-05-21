#include "StateManager.h"

#include <Arduino.h>
#include <cmath>

#define TAPWATER_LOWER_LIMIT 2000 // values under this are ignored
#define TAPWATER_RUNNING_UPPER 5500 // water can be considered running under this limit

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

StateManager::LIGHTINTENSITY StateManager::getLight() const {
    return this->m_lightIntensity;
}

StateManager::TEMP_STATE StateManager::getTempState() const {
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

StateManager* StateManager::updateCapsenseWaterTap(uint16_t measurementVal, unsigned long timestamp) {
    this->m_tapWaterCapSenseVal = measurementVal;
    return this;
}