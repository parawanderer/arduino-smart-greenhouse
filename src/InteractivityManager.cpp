#include "InteractivityManager.h"
#include <Arduino.h>

#define SERVO_WINDOW_OPEN 60
#define SERVO_WINDOW_CLOSED 30

InteractivityManager::InteractivityManager(StateManager& stateManager, int servoPin) : m_stateManager(stateManager), m_servoPin(servoPin)
{
    this->m_servo.attach(this->m_servoPin);
}

void InteractivityManager::openWindow() {
    if (this->m_stateManager.isWindowOpen()) {
        return;
    }

    Serial.println("Window being opened...");
    this->m_servo.write(SERVO_WINDOW_OPEN);
}

void InteractivityManager::closeWindow() {
    if (!this->m_stateManager.isWindowOpen()) {
        return;
    }

    Serial.println("Window being closed...");
    this->m_servo.write(SERVO_WINDOW_CLOSED);
}

void InteractivityManager::openWaterTap() {
    if (this->m_stateManager.isWaterRunning()) return;
    // TODO:
}

void InteractivityManager::closeWaterTap() {
    if (!this->m_stateManager.isWaterRunning()) return;
    // TODO:
}