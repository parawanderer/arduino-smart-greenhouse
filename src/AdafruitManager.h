#ifndef ADAFRUITMANAGER_H
#define ADAFRUITMANAGER_H

#include "AdafruitIO_WiFi.h"
#include "AdafruitIO_Data.h"
#include "AdafruitIO_Feed.h"

#include "StateManager.h"
#include "DataManager.h"

class AdafruitManager {
    public:
        AdafruitManager(AdafruitIO& ioItem, StateManager& stateManager, DataManager& dataManager);

        void init(AdafruitIODataCallbackType cbEnableFeed, AdafruitIODataCallbackType cbWindowFeed, AdafruitIODataCallbackType cbTapFeed, AdafruitIODataCallbackType cbTempFeed);
        void handleLoop(unsigned long timestamp);

        bool isOverrideEnabled() const;
        bool getOverrideOpenWindow() const;
        bool getOverrideOpenTap() const;
        int getOverrideConfigureTemp() const;


        void handleOverrideEnableFeed(AdafruitIO_Data *data);
        void handleOverrideWindowFeed(AdafruitIO_Data *data);
        void handleOverrideTapFeed(AdafruitIO_Data *data);
        void handleOverrideTempFeed(AdafruitIO_Data *data);

    private:
        AdafruitIO& m_io;
        StateManager& m_stateManager;
        DataManager& m_dataManager;

        unsigned long m_lastSendTime = ULONG_MAX;

        AdafruitIO_Feed* m_rawLightIntensityFeed;
        AdafruitIO_Feed* m_convertedLightIntensityFeed;


        AdafruitIO_Feed* m_lightHistory1DayFeed;
        AdafruitIO_Feed* m_lightHistory2DaysFeed;
        AdafruitIO_Feed* m_lightHistory3DaysFeed;
        unsigned char m_last1DayValue = -1;
        unsigned char m_last2DaysValue = -1;
        unsigned char m_last3DaysValue = -1;


        AdafruitIO_Feed* m_temperatureCurrentFeed;
        AdafruitIO_Feed* m_temperatureConfiguredFeed;
        int m_lastConfiguredTemp = 0;
        AdafruitIO_Feed* m_temperatureStatusFeed;
        char * m_lastStatusIcon = nullptr;


        AdafruitIO_Feed* m_doorFeed;
        AdafruitIO_Feed* m_windowFeed;
        AdafruitIO_Feed* m_tapFeed;
        bool m_lastDoorStatus = false;
        bool m_lastWindowStatus = false;
        bool m_lastTapStatus = false;


        AdafruitIO_Feed* m_soilMoistureFeed;
        unsigned char m_lastSoilMoistureLevel = -1;


        AdafruitIO_Feed* m_overrideEnableFeed;
        AdafruitIO_Feed* m_overrideWindowOpenFeed;
        AdafruitIO_Feed* m_overrideTapOpenFeed;
        AdafruitIO_Feed* m_overrideConfTempFeed;
        bool m_isOverrideEnabled = false;
        bool m_overrideOpenWindow = false;
        bool m_overrideOpenTap = false;
        int m_overrideTemp = 0;
        
        void handleTemperature();
        void handleLight();
        void handleBooleanValues();
        void handleSoil();
};

#endif