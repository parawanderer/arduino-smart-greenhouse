#include "DebugModeManager.h"

#include <Arduino.h>
#include "ArrayUtil.h"

DebugModeManager::DebugModeManager(unsigned char button1, unsigned char button2) : m_button1(button1), m_button2(button2)
{}

void DebugModeManager::onButtonPressed(unsigned char button) {
    shiftLeft(this->m_debugActivationArray, DEBUG_BUTTON_HISTORY_SIZE, button);

    bool isToggle =  this->m_debugActivationArray[0] == m_button1
                                && this->m_debugActivationArray[1] == m_button1
                                && this->m_debugActivationArray[2] == m_button2
                                && this->m_debugActivationArray[3] == m_button2
                                && this->m_debugActivationArray[4] == m_button1; // matches pattern

    if (isToggle) {
        this->m_debugModeEnabled = !this->m_debugModeEnabled;
    }
}

bool DebugModeManager::isDebugModeEnabled() {
    return this->m_debugModeEnabled;
}