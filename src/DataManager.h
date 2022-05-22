#ifndef DATAMANAGER_H
#define DATAMANAGER_H
#include "StateManager.h"

class DataManager {
    public:
        DataManager(StateManager& stateManager);

        void init();

    private:
        StateManager& m_stateManager;
};

#endif