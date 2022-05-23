#include "InteractivityManager.h"
#include <Arduino.h>

#define SERVO_WINDOW_OPEN 90
#define SERVO_WINDOW_CLOSED 30

#define STEPPER_RPM 8

InteractivityManager::InteractivityManager(StateManager& stateManager, int servoPin, int stepsPerRevolution, int stepperPin1, int stepperPin2, int stepperPin3, int stepperPin4) 
:   m_stateManager(stateManager), 
    m_servoPin(servoPin), 
    m_stepsPerRevolution(stepsPerRevolution), 
    m_stepperPin1(stepperPin1), 
    m_stepperPin2(stepperPin2), 
    m_stepperPin3(stepperPin3), 
    m_stepperPin4(stepperPin4),
    m_stepper(stepsPerRevolution, stepperPin1, stepperPin3, stepperPin2, stepperPin4)
{
    this->m_servo.attach(this->m_servoPin);
    this->m_stepper.setSpeed(STEPPER_RPM);
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
    

    Serial.println("Water tap being opened...");
    this->m_stepper.step(this->m_stepsPerRevolution);
}

void InteractivityManager::closeWaterTap() {
    if (!this->m_stateManager.isWaterRunning()) return;
    
    Serial.println("Water tap being closed...");
    this->m_stepper.step(-this->m_stepsPerRevolution);
}