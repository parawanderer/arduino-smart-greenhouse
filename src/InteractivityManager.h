#ifndef INTERACTIVITYMANAGER_H
#define INTERACTIVITYMANAGER_H
#include "StateManager.h"

#include <ESP32Servo.h>
#include <Stepper.h>

class InteractivityManager {
    public:
        InteractivityManager(StateManager& stateManager, int servoPin, int stepsPerRevolution, int stepperPin1, int stepperPin2, int stepperPin3, int stepperPin4);

        void openWindow();
        void openWaterTap();
        void closeWindow();
        void closeWaterTap();
    
    private:
        StateManager& m_stateManager;

        int m_servoPin;
        Servo m_servo;

        int m_stepsPerRevolution;
        int m_stepperPin1;
        int m_stepperPin2;
        int m_stepperPin3;
        int m_stepperPin4;
        Stepper m_stepper;
};

#endif