#include "DataManager.h"
#include <math.h>

#define HR_IN_DAY 24
#define AVERAGE_LIGHT_HISTORY_SIZE 14

#define UNKNOWN_STR "???"
#define MON_STR  "Mon"
#define TUE_STR  "Tue"
#define WED_STR  "Wed"
#define THU_STR  "Thu"
#define FRI_STR  "Fri"
#define SAT_STR  "Sat"
#define SUN_STR  "Sun"

#define UNSET_LIGHT_VAL (byte)-1

const char* WEEKDAYS[] = {
    SUN_STR, MON_STR, TUE_STR, WED_STR, THU_STR, FRI_STR, SAT_STR
};


DataManager::DataManager()
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

void DataManager::onNewLightValue(unsigned long timeSinceStartup, LIGHTINTENSITY lightIntensity) {
    struct tm time = this->mapToRealTime(timeSinceStartup);

    if (time.tm_hour != this->m_lastHour) {
        // if is next hour : update last hour average in tracker
        this->m_dayLightAverages[this->m_lastHour - 1] = round(this->m_currentHrAvgLight);
        this->m_nValuesForCurrentHrAvg = 0;
        this->m_currentHrAvgLight = 0.0;
    }
    else if (time.tm_min > this->m_lastMinute) {
        // if is next minute : add this to our day average
        this->m_currentHrAvgLight = ((this->m_currentHrAvgLight * this->m_nValuesForCurrentHrAvg) + (int) lightIntensity) / (this->m_nValuesForCurrentHrAvg + 1);
        this->m_nValuesForCurrentHrAvg++;
    }

    if (time.tm_mday != this->m_currentDay) {
        this->addHistoryEntry();
    }

    this->m_currentDay = time.tm_mday;
    this->m_lastHour = time.tm_hour;
    this->m_lastMinute = time.tm_min;
}

void DataManager::_debugPrintData() {
    Serial.printf(" =========== DATAMANAGER: =========== \n");
    Serial.printf("Current Day: %d   Last Hour: %d    Last Minute: %d\n", this->m_currentDay, this->m_lastHour, this->m_lastMinute);
    Serial.printf("Current Average for hour: %02.4f    Based on n Values: %d\n", this->m_currentHrAvgLight, this->m_nValuesForCurrentHrAvg);

    Serial.printf("HOURLY AVERAGES FOR DAY: ");
    for (int i = 0; i < HR_IN_DAY; ++i) {
        Serial.print(this->m_dayLightAverages[i]);
        Serial.print(", ");
    }
    Serial.println("");
    Serial.printf("HISTORY SIZE: %d\n", this->m_historyDailyLightHours.size());
    Serial.printf("HISTORY ITEMS: ");
    LinkedListNode<byte>* iter = this->m_historyDailyLightHours.getIterator();
    while (iter != nullptr) {
        Serial.print(iter->m_item);
        Serial.print(", ");
        iter = iter->m_next;
    }
    Serial.println("");

    Serial.printf(" ====================== \n");
}

HistoryData DataManager::getHistoryLast3Days() {
    LinkedListNode<byte>* iter = this->m_historyDailyLightHours.getIterator();
    int i = 0;
    int target = this->m_historyDailyLightHours.size() - 3; // last 3 items
    
    while (iter != nullptr && i < target) {
        iter = iter->m_next;
        i++;
    }

    HistoryData data = {
        .m_val1 = UNSET_LIGHT_VAL,
        .m_weekDay1 = UNKNOWN_STR,
        .m_val2 = UNSET_LIGHT_VAL,
        .m_weekDay2 = UNKNOWN_STR,
        .m_val3 = UNSET_LIGHT_VAL,
        .m_weekDay3 = UNKNOWN_STR
    };

    if (iter == nullptr) return data;

    struct tm lastTime = this->mapToRealTime(this->m_lastProcessedTimestamp);
    lastTime.tm_mday -= 1;
    mktime(&lastTime);
    const char* weekday1 = WEEKDAYS[lastTime.tm_wday];
    lastTime.tm_mday -= 1;
    mktime(&lastTime);
    const char* weekday2 = WEEKDAYS[lastTime.tm_wday];
    lastTime.tm_mday -= 1;
    mktime(&lastTime);
    const char* weekday3 = WEEKDAYS[lastTime.tm_wday];

    // first item
    data.m_val1 = iter->m_item;
    data.m_weekDay1 = weekday1;
    
    iter = iter->m_next;
    if (iter == nullptr) return data;

    // second item
    data.m_val2 = iter->m_item;
    data.m_weekDay2 = weekday2;
    
    iter = iter->m_next;
    if (iter == nullptr) return data;
    // final item

    data.m_val3 = iter->m_item;
    data.m_weekDay3 = weekday3;

    return data;
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

void DataManager::addHistoryEntry() {
    byte lightHoursForDay = 0;

    for (int i = 0; i < HR_IN_DAY; ++i) {
        if (this->m_dayLightAverages[i] == -1) continue; // not calculated
        if (this->m_dayLightAverages[i] >= 2) lightHoursForDay++; // I will count everything above light intensity lvl 2 as "light hours"
    }

    if (this->m_historyDailyLightHours.size() > AVERAGE_LIGHT_HISTORY_SIZE) {
        this->m_historyDailyLightHours.removeFirst();
    }

    this->m_historyDailyLightHours.insert(lightHoursForDay);
}