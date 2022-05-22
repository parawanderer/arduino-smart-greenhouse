
#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <Arduino.h>
#include "LIGHTINTENSITY.h"
#include "TEMP_STATE.h"

#define WT_SAMPLES_CNT 1000

class StateManager {
    public:
        StateManager();

        bool isDoorOpen() const;
        bool isWindowOpen() const;

        int getConfiguredTemperatue() const;
        float getTrueTemperature() const;
        TEMP_STATE getTempState() const;

        LIGHTINTENSITY getLight() const;

        bool isWaterRunning() const;
        
        StateManager* setDoorOpen(bool state);
        StateManager* setWindowOpen(bool state);
        StateManager* setConfiguredTemperature(int temperature);
        StateManager* setLightRaw(int lightVal, int lightMax);
        StateManager* setTemperature(float temp);
        StateManager* updateCapsenseWaterTap(uint16_t measurementVal, unsigned long timestamp);


    private:
        // je zou dit eventueel efficienter kunnen opslaan in een integer met masks. 
        // Ik zal dat hier momenteel niet doen.
        bool m_isDoorOpen;
        bool m_isWindowOpen;

        int m_configuredTemperature;
        
        float m_temperature; // true temperature

        // ook dit zou in een integer kunnen gezet worden
        LIGHTINTENSITY m_lightIntensity;

        uint16_t m_tapWaterCapSenseVal = 0;
};

#endif