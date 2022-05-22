#ifndef INTERACTIVITYMANAGER_H
#define INTERACTIVITYMANAGER_H
#include "StateManager.h"

#include <ESP32Servo.h>

class InteractivityManager {
    public:
        InteractivityManager(StateManager& stateManager, int servoPin);

        void openWindow();
        void openWaterTap();
        void closeWindow();
        void closeWaterTap();
    
    private:
        StateManager& m_stateManager;

        int m_servoPin;
        Servo m_servo;
};

#endif