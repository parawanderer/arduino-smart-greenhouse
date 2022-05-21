
#ifndef STATEMANAGER_H
#define STATEMANAGER_H

class StateManager {
    public:
        StateManager();

        bool isDoorOpen();
        bool isWindowOpen();

        StateManager* setDoorOpen(bool state);
        StateManager* setWindowOpen(bool state);

    private:
        bool m_isDoorOpen;
        bool m_isWindowOpen;
};

#endif