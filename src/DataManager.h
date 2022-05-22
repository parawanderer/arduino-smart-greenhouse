#ifndef DATAMANAGER_H
#define DATAMANAGER_H
#include "AdafruitIO_WiFi.h"

#include "LinkedList.h"
#include "StateManager.h"

class DataManager {
    public:
        DataManager(StateManager& stateManager, AdafruitIO& io);

        void init(struct tm baseTime, unsigned long msSinceStartup);

        void onNewLightValue(unsigned long timeSinceStartup);

        void _debugPrintData();

    private:
        StateManager& m_stateManager;
        AdafruitIO& m_io;

        struct tm m_baseTime;
        unsigned long m_lastProcessedTimestamp = 0;

        int m_currentDay;
        int m_lastHour;
        int m_lastMinute;

        LinkedList<byte> m_2WeekDailyLightHours;
        int m_dayLightAverages[24];

        double m_currentHrAvgLight = 0.0;
        int m_nValuesForCurrentHrAvg = 0;

        struct tm mapToRealTime(unsigned long timeSinceStartup);
        void addNew2WeekEntry();
};

#endif