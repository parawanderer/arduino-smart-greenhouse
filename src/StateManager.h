
#ifndef STATEMANAGER_H
#define STATEMANAGER_H



class StateManager {

    enum class LIGHTINTENSITY {
        NIGHT = 0,
        VERY_DARK = 1,
        DARK = 2,
        AVERAGE = 3,
        LIGHT = 4,
        VERY_LIGHT = 5
    };

    public:
        StateManager();

        bool isDoorOpen() const;
        bool isWindowOpen() const;

        int getConfiguredTemperatue() const;

        StateManager::LIGHTINTENSITY getLight() const;
        
        StateManager* setDoorOpen(bool state);
        StateManager* setWindowOpen(bool state);
        StateManager* setConfiguredTemperature(int temperature);
        StateManager* setLightRaw(int lightVal, int lightMax);

    private:
        // je zou dit eventueel efficienter kunnen opslaan in een integer met masks. 
        // Ik zal dat hier momenteel niet doen.
        bool m_isDoorOpen;
        bool m_isWindowOpen;

        int m_configuredTemperature;

        // ook dit zou in een integer kunnen gezet worden
        StateManager::LIGHTINTENSITY m_lightIntensity;
};

#endif