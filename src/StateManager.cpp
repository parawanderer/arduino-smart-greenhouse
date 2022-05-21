#include "StateManager.h"

StateManager::StateManager() : m_isDoorOpen(false), m_isWindowOpen(false) {
}

bool StateManager::isDoorOpen() {
    return this->m_isDoorOpen;
}

bool StateManager::isWindowOpen() {
    return this->m_isWindowOpen;
}

StateManager* StateManager::setDoorOpen(bool state) {
    this->m_isDoorOpen = state;
    return this;
}

StateManager* StateManager::setWindowOpen(bool state) {
    this->m_isWindowOpen = state;
    return this;
}