#include "AdafruitManager.h"
#include "Credentials.h"

#include <Arduino.h>

#define WAIT_BETWEEN_PACKAGES 15000      // ms

#define NO_VALUE  (byte) -1

#define FEED_LIGHTVALUES_RAW        "SlimmeSerre_feed_light_raw"
#define FEED_LIGHTVALUES_CONVERTED  "SlimmeSerre_feed_light_converted"
#define FEED_LIGHTHISTORY_YESTERDAY "SlimmeSerre_feed_light_history_yesterday"
#define FEED_LIGHTHISTORY_2DAYS     "SlimmeSerre_feed_light_history_2days_ago"
#define FEED_LIGHTHISTORY_3DAYS     "SlimmeSerre_feed_light_history_3days_ago"

#define FEED_CURRENT_TEMPERATURE    "SlimmeSerre_feed_current_temp"
#define FEED_CONFIGURED_TEMPERATURE "SlimmeSerre_feed_config_temp"
#define FEED_TEMPERATURE_STATE      "SlimmeSerre_feed_temp_state"

#define FEED_DOOR_OPEN              "SlimmeSerre_feed_door_open"
#define FEED_WATER_TAP_OPEN         "SlimmeSerre_feed_water_tap_open"
#define FEED_WINDOW_OPEN            "SlimmeSerre_feed_window_open"

#define FEED_SOIL_STATUS            "SlimmeSerre_feed_soil_status"

#define FEED_OVERRIDE               "SlimmeSerre_feed_do_override"
#define FEED_OVERRIDE_OPEN_WINDOW   "SlimmeSerre_feed_override_open_window"
#define FEED_OVERRIDE_OPEN_TAP      "SlimmeSerre_feed_override_open_tap"
#define FEED_OVERRIDE_CONF_TEMP     "SlimmeSerre_feed_override_conf_temp"

char THUMBS_UP[] = "thumbs-up";
char THUMBS_O_UP[] = "thumbs-o-up";
char WARNING[] = "warning";
char W_NIGHT_CLEAR[] = "w:night-clear";
char W_DAY_CLOUDY[] = "w:day-cloudy";
char W_DAY_SUNNY_OVERCAST[] = "w:day-sunny-overcast";
char W_DAY_SUNNY[] = "w:day-sunny";
char W_SOLAR_ECLIPSE[] = "w:solar-eclipse";

char* TEMP_STATUS_ICONS[] = {
    THUMBS_UP,
    THUMBS_O_UP,
    WARNING
};

char* LIGHT_LEVEL_ICONS[] = {
    W_NIGHT_CLEAR,
    W_DAY_CLOUDY,
    W_DAY_SUNNY_OVERCAST,
    W_DAY_SUNNY,
    W_SOLAR_ECLIPSE
};

AdafruitManager::AdafruitManager(AdafruitIO& ioItem, StateManager& stateManager, DataManager& dataManager) 
:   m_io(ioItem), 
    m_stateManager(stateManager),
    m_dataManager(dataManager),

    m_rawLightIntensityFeed(ioItem.feed(FEED_LIGHTVALUES_RAW)),
    m_convertedLightIntensityFeed(ioItem.feed(FEED_LIGHTVALUES_CONVERTED)),

    m_lightHistory1DayFeed(ioItem.feed(FEED_LIGHTHISTORY_YESTERDAY)),
    m_lightHistory2DaysFeed(ioItem.feed(FEED_LIGHTHISTORY_2DAYS)),
    m_lightHistory3DaysFeed(ioItem.feed(FEED_LIGHTHISTORY_3DAYS)),

    m_temperatureCurrentFeed(ioItem.feed(FEED_CURRENT_TEMPERATURE)),
    m_temperatureConfiguredFeed(ioItem.feed(FEED_CONFIGURED_TEMPERATURE)),
    m_temperatureStatusFeed(ioItem.feed(FEED_TEMPERATURE_STATE)),

    m_doorFeed(ioItem.feed(FEED_DOOR_OPEN)),
    m_windowFeed(ioItem.feed(FEED_WINDOW_OPEN)),
    m_tapFeed(ioItem.feed(FEED_WATER_TAP_OPEN)),

    m_soilMoistureFeed(ioItem.feed(FEED_SOIL_STATUS)),

    m_overrideEnableFeed(ioItem.feed(FEED_OVERRIDE)),
    m_overrideWindowOpenFeed(ioItem.feed(FEED_OVERRIDE_OPEN_WINDOW)),
    m_overrideTapOpenFeed(ioItem.feed(FEED_OVERRIDE_OPEN_TAP)),
    m_overrideConfTempFeed(ioItem.feed(FEED_OVERRIDE_CONF_TEMP))
{}

void AdafruitManager::init(AdafruitIODataCallbackType cbEnableFeed, AdafruitIODataCallbackType cbWindowFeed, AdafruitIODataCallbackType cbTapFeed, AdafruitIODataCallbackType cbTempFeed) {
    Serial.printf("WIFI - ADAFRUIT.IO: Connecting as %s...\n", ADAFRUITIO_USERNAME);
    this->m_io.connect();

    while (this->m_io.status() < AIO_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    Serial.println(this->m_io.statusText());

    this->m_overrideEnableFeed->onMessage(cbEnableFeed);
    this->m_overrideWindowOpenFeed->onMessage(cbWindowFeed);
    this->m_overrideTapOpenFeed->onMessage(cbTapFeed);
    this->m_overrideConfTempFeed->onMessage(cbTempFeed);

    this->m_overrideEnableFeed->get();
    this->m_overrideWindowOpenFeed->get();
    this->m_overrideTapOpenFeed->get();
    this->m_overrideConfTempFeed->get();
}

void AdafruitManager::handleLoop(unsigned long timestamp) {
    if (timestamp < this->m_lastSendTime && (timestamp + (ULONG_MAX - this->m_lastSendTime)) < WAIT_BETWEEN_PACKAGES) return;
    else if (timestamp >= this->m_lastSendTime && timestamp - this->m_lastSendTime < WAIT_BETWEEN_PACKAGES) return;

    // io.run(); is required for all sketches.
    // it should always be present at the top of your loop
    // function. it keeps the client connected to
    // io.adafruit.com, and processes any incoming data.
    this->m_io.run();

    Serial.println("Sending data to Adafruit.IO....");

    this->handleLight();
    this->handleTemperature();
    this->handleBooleanValues();
    this->handleSoil();

    Serial.println("Done Sending data to Adafruit.IO");

    this->m_lastSendTime = timestamp;
}

bool AdafruitManager::isOverrideEnabled() const {
    return this->m_isOverrideEnabled;
}

bool AdafruitManager::getOverrideOpenTap() const {
    return this->m_overrideOpenTap;
}

bool AdafruitManager::getOverrideOpenWindow() const {
    return this->m_overrideOpenWindow;
}

int AdafruitManager::getOverrideConfigureTemp() const {
    return this->m_overrideTemp;
}

void AdafruitManager::handleTemperature() { 
    // temperature
    this->m_temperatureCurrentFeed->save((double) this->m_stateManager.getTrueTemperature());

    int configuredTemp = this->m_stateManager.getConfiguredTemperatue();
    if (configuredTemp != 0 && this->m_lastConfiguredTemp != configuredTemp) {
        this->m_temperatureConfiguredFeed->save(configuredTemp);
        configuredTemp = this->m_lastConfiguredTemp;
    }
    
    char * statusIcon = TEMP_STATUS_ICONS[(int) this->m_stateManager.getTempState()];
    if (statusIcon != this->m_lastStatusIcon) {
        this->m_temperatureStatusFeed->save(statusIcon);
        this->m_lastStatusIcon = statusIcon;
    }
}

void AdafruitManager::handleLight() {
    // light
    this->m_rawLightIntensityFeed->save(this->m_stateManager.getLightRaw());
    this->m_convertedLightIntensityFeed->save((int) this->m_stateManager.getLight());

    // light icons history
    HistoryData history = this->m_dataManager.getHistoryLast3Days();

    if (history.m_val1 != NO_VALUE && this->m_last1DayValue != history.m_val1) {
        this->m_lightHistory1DayFeed->save(LIGHT_LEVEL_ICONS[(int) history.m_val1]);
        this->m_last1DayValue = history.m_val1;
    }

    if (history.m_val2 != NO_VALUE && this->m_last2DaysValue != history.m_val2) {
        this->m_lightHistory2DaysFeed->save(LIGHT_LEVEL_ICONS[(int) history.m_val2]);
        this->m_last2DaysValue = history.m_val2;
    }

    if (history.m_val3 != NO_VALUE && this->m_last3DaysValue != history.m_val3) {
        this->m_lightHistory3DaysFeed->save(LIGHT_LEVEL_ICONS[(int) history.m_val3]);
        this->m_last3DaysValue = history.m_val3;
    }
}

void AdafruitManager::handleBooleanValues() {
    bool doorOpen = this->m_stateManager.isDoorOpen();
    if (doorOpen != this->m_lastDoorStatus) {
        this->m_doorFeed->save(doorOpen);
        this->m_lastDoorStatus = doorOpen;
    }

    bool windowOpen = this->m_stateManager.isWindowOpen();
    if (windowOpen != this->m_lastWindowStatus) {
        this->m_windowFeed->save(windowOpen);
        this->m_lastWindowStatus = windowOpen;
    }

    bool tapOpen = this->m_stateManager.isWaterRunning();
    if (tapOpen != this->m_lastTapStatus) {
        this->m_tapFeed->save(tapOpen);
        this->m_lastTapStatus = tapOpen;
    }
}

void AdafruitManager::handleSoil() {
    unsigned char level = (unsigned char) this->m_stateManager.getSoilMoistureLevel();
    if (level != this->m_lastSoilMoistureLevel) {
        this->m_soilMoistureFeed->save(level);
        this->m_lastSoilMoistureLevel = level;
    }
}

void AdafruitManager::handleOverrideEnableFeed(AdafruitIO_Data *data) {
    this->m_isOverrideEnabled = data->toBool();
    Serial.printf("Received new global override enable option from Adafruit.IO: %d\n", this->m_isOverrideEnabled);
}

void AdafruitManager::handleOverrideWindowFeed(AdafruitIO_Data *data) {
    this->m_overrideOpenWindow = data->toBool();
    Serial.printf("Received new override window open option from Adafruit.IO: %d (global override enabled: %d)\n", this->m_overrideOpenWindow, this->m_isOverrideEnabled);
}

void AdafruitManager::handleOverrideTapFeed(AdafruitIO_Data *data) {
    this->m_overrideOpenTap = data->toBool();
    Serial.printf("Received new override tap open option from Adafruit.IO: %d (global override enabled: %d)\n", this->m_overrideOpenTap, this->m_isOverrideEnabled);
}

void AdafruitManager::handleOverrideTempFeed(AdafruitIO_Data *data) {
    this->m_overrideTemp = data->toInt();
    Serial.printf("Received new override config temp from Adafruit.IO: %d (global override enabled: %d)\n", this->m_overrideTemp, this->m_isOverrideEnabled);
}