#ifndef DATAMANAGER_H
#define DATAMANAGER_H
#include "AdafruitIO_WiFi.h"

#include "LinkedList.h"
#include "StateManager.h"

struct HistoryData {
    byte m_val1;
    const char* m_weekDay1;
    byte m_val2;
    const char* m_weekDay2;
    byte m_val3;
    const char* m_weekDay3;
};

class DataManager {
    public:
        DataManager(AdafruitIO& io);

        void init(struct tm baseTime, unsigned long msSinceStartup);

        void onNewLightValue(unsigned long timeSinceStartup, LIGHTINTENSITY lightIntensity);
        HistoryData getHistoryLast3Days();

        void _debugPrintData();

    private:
        AdafruitIO& m_io;

        struct tm m_baseTime;
        unsigned long m_lastProcessedTimestamp = 0;

        int m_currentDay;
        int m_lastHour;
        int m_lastMinute;

        LinkedList<byte> m_historyDailyLightHours;
        int m_dayLightAverages[24];

        double m_currentHrAvgLight = 0.0;
        int m_nValuesForCurrentHrAvg = 0;

        struct tm mapToRealTime(unsigned long timeSinceStartup);
        void addHistoryEntry();
};

#endif