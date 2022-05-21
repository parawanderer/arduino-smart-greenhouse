#ifndef INTERACTIVITYMANAGER_H
#define INTERACTIVITYMANAGER_H
#include "StateManager.h"

class InteractivityManager {
    public:
        InteractivityManager(StateManager& stateManager);

        void openWindow();
        void openWaterTap();
        void closeWindow();
        void closeWaterTap();
    
    private:
        StateManager& m_stateManager;
};

#endif