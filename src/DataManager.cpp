#include "DataManager.h"
#include <math.h>

#define HR_IN_DAY 24
#define AVERAGE_LIGHT_HISTORY_SIZE 14

DataManager::DataManager(StateManager& stateManager, AdafruitIO& io) : m_stateManager(stateManager), m_io(io) 
{
    for (int i = 0; i < HR_IN_DAY; ++i) {
        this->m_dayLightAverages[i] = -1;
    }
}

void DataManager::init(struct tm baseTime, unsigned long msSinceStartup) {
    this->m_baseTime = baseTime;

    this->m_baseTime.tm_sec -= (msSinceStartup / 1000); // roll back to true system start time
    mktime(&this->m_baseTime);

    this->m_currentDay = this->m_baseTime.tm_mday;
    this->m_lastHour = this->m_baseTime.tm_hour;
    this->m_lastMinute = this->m_baseTime.tm_min;
}

void DataManager::onNewLightValue(unsigned long timeSinceStartup) {
    struct tm time = this->mapToRealTime(timeSinceStartup);

    if (time.tm_hour != this->m_lastHour) {
        // if is next hour
        // update last hour average in tracker
        this->m_dayLightAverages[this->m_lastHour - 1] = round(this->m_currentHrAvgLight);
        this->m_nValuesForCurrentHrAvg = 0;
        this->m_currentHrAvgLight = 0.0;
    }
    else if (time.tm_min > this->m_lastMinute) { // if is next minute
        byte currentLightIntensity = (byte) this->m_stateManager.getLight();

        // add this to our day average
        this->m_currentHrAvgLight = ((this->m_currentHrAvgLight * this->m_nValuesForCurrentHrAvg) + currentLightIntensity) / (this->m_nValuesForCurrentHrAvg + 1);
        this->m_nValuesForCurrentHrAvg++;
    }

    if (time.tm_mday != this->m_currentDay) {
        this->addNew2WeekEntry();
    }

    this->m_currentDay = time.tm_mday;
    this->m_lastHour = time.tm_hour;
    this->m_lastMinute = time.tm_min;
}

void DataManager::_debugPrintData() {
    Serial.printf(" =========== DATAMANAGER: =========== \n");
    Serial.printf("Current Day: %d   Last Hour: %d    Last Minute: %d\n", this->m_currentDay, this->m_lastHour, this->m_lastMinute);
    Serial.printf("Current Average for hour: %02.4f    Based on Values: %d\n", this->m_currentHrAvgLight, this->m_nValuesForCurrentHrAvg);

    Serial.printf("HOURLY AVERAGES FOR DAY: ");
    for (int i = 0; i < HR_IN_DAY; ++i) {
        Serial.print(this->m_dayLightAverages[i]);
        Serial.print(", ");
    }
    Serial.println("");
    Serial.printf("HISTORY SIZE: %d\n", this->m_2WeekDailyLightHours.size());
    Serial.printf(" ====================== \n");
}

struct tm DataManager::mapToRealTime(unsigned long timeSinceStartup) {
    if (timeSinceStartup < this->m_lastProcessedTimestamp) {
        // overflow: move base offset
        this->m_baseTime.tm_sec += ULONG_MAX;
        mktime(&this->m_baseTime);
    }

    struct tm baseTime = this->m_baseTime;
    baseTime.tm_sec += (timeSinceStartup / 1000); // convert to seconds
    mktime(&baseTime);
    this->m_lastProcessedTimestamp = timeSinceStartup;
    return baseTime;
}

void DataManager::addNew2WeekEntry() {
    byte lightHoursForDay = 0;

    for (int i = 0; i < HR_IN_DAY; ++i) {
        if (this->m_dayLightAverages[i] == -1) continue; // not calculated
        if (this->m_dayLightAverages[i] >= 2) lightHoursForDay++; // I will count everything above light intensity lvl 2 as "light hours"
    }

    if (this->m_2WeekDailyLightHours.size() > AVERAGE_LIGHT_HISTORY_SIZE) {
        this->m_2WeekDailyLightHours.removeFirst();
    }

    this->m_2WeekDailyLightHours.insert(lightHoursForDay);
}