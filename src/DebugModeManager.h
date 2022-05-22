#ifndef DEBUGMODEMANAGER_H
#define DEBUGMODEMANAGER_H

#define DEBUG_BUTTON_HISTORY_SIZE  5
#define DEBUG_BUTTON_HISTORY_EMPTYVAL (unsigned char) -1

class DebugModeManager {

    public:
        DebugModeManager(unsigned char button1, unsigned char button2);

        void onButtonPressed(unsigned char button);
        bool isDebugModeEnabled();

    private: 
        unsigned char m_button1; 
        unsigned char m_button2;
        unsigned char m_debugActivationArray[DEBUG_BUTTON_HISTORY_SIZE] = {DEBUG_BUTTON_HISTORY_EMPTYVAL, DEBUG_BUTTON_HISTORY_EMPTYVAL, DEBUG_BUTTON_HISTORY_EMPTYVAL, DEBUG_BUTTON_HISTORY_EMPTYVAL, DEBUG_BUTTON_HISTORY_EMPTYVAL};
        bool m_buttonHistoryChanged = false;
        bool m_debugModeEnabled = false;
};

#endif