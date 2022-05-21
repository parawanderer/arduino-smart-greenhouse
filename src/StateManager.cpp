#include "StateManager.h"

StateManager::StateManager() : m_isDoorOpen(false), m_isWindowOpen(false), m_configuredTemperature(0.0) {
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
