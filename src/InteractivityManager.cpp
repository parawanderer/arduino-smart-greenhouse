#include "InteractivityManager.h"

InteractivityManager::InteractivityManager(StateManager& stateManager) : m_stateManager(stateManager)
{}

void InteractivityManager::openWindow() {
    if (this->m_stateManager.isWindowOpen()) return;
    // TODO:
}

void InteractivityManager::closeWindow() {
    if (!this->m_stateManager.isWindowOpen()) return;
    // TODO:
}

void InteractivityManager::openWaterTap() {
    if (this->m_stateManager.isWaterRunning()) return;
    // TODO:
}

void InteractivityManager::closeWaterTap() {
    if (!this->m_stateManager.isWaterRunning()) return;
    // TODO:
}