
#ifndef STATEMANAGER_H
#define STATEMANAGER_H

class StateManager {
    public:
        StateManager();

        bool isDoorOpen() const;
        bool isWindowOpen() const;

        int getConfiguredTemperatue() const;
        
        StateManager* setDoorOpen(bool state);
        StateManager* setWindowOpen(bool state);
        StateManager* setConfiguredTemperature(int temperature);

    private:
        // je zou dit eventueel efficienter kunnen opslaan in een integer met masks. 
        // Ik zal dat hier momenteel niet doen.
        bool m_isDoorOpen;
        bool m_isWindowOpen;

        int m_configuredTemperature;
};

#endif