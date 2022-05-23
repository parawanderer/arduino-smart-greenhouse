#include <Arduino.h>
#include <WiFi.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#include <driver\touch_pad.h>

#include "Credentials.h"
#include "config.h"

#include "AdafruitIO_WiFi.h"
#include "AdafruitIO_Data.h"
#include "AdafruitIO_Feed.h"

#include "StateManager.h"
#include "DisplayManager.h"
#include "InteractivityManager.h"
#include "DataManager.h"
#include "AdafruitManager.h"
#include "DebugModeManager.h"

#include "PotTemperatureUtil.h"

#define DOOR_BTN_PIN 0
#define WINDOW_BTN_PIN 35
#define POT_PIN 36
#define LIGHT_SENSOR_PIN 39
#define SERVO_PIN 27

#define CAPSENSE_WATERTAP_TOUCHNUM TOUCH_PAD_NUM4 // GPIO13
#define CAPSENSE_SOILMOISTURE_TOUCHNUM TOUCH_PAD_NUM5 // GPIO12

#define STEPPER_PIN_1 26
#define STEPPER_PIN_2 25
#define STEPPER_PIN_3 33
#define STEPPER_PIN_4 32
#define STEPS_PER_REVOLUTION 2038


#define ADC_MAX_VAL 4095

// for Adafruit_BMP280
#define SEALEVELPRESSURE_HPA (1013.25)


// time server setup
#define NTP_SERVER "0.be.pool.ntp.org"
#define GMT_OFFSET_SEC 0
#define DAYLIGHT_OFFSET_SEC 3600
#define TIME_CORRECTION_HOURS 1 // we need GMT + 1 for belgium


StateManager state = StateManager();
InteractivityManager interactivity = InteractivityManager(state, SERVO_PIN, STEPS_PER_REVOLUTION, STEPPER_PIN_1, STEPPER_PIN_2, STEPPER_PIN_3, STEPPER_PIN_4);
DataManager data = DataManager();
AdafruitManager adafruit = AdafruitManager(io, state, data);
DisplayManager display = DisplayManager(state, data, adafruit);
DebugModeManager debugMode = DebugModeManager(DOOR_BTN_PIN, WINDOW_BTN_PIN);

PotTemperatureUtil potUtil = PotTemperatureUtil((float) ADC_MAX_VAL);


Adafruit_BMP280 bmp = Adafruit_BMP280();


void initButtons();
void initPot();
void initTempSensor();
void initCapsense();
void initWifi();
void initWifiTime();
void initCapsenseItem(touch_pad_t touch_num);

void onChangeDoorButton();
void onChangeWindowButton();

void handlePot();
void handleLightSensor(unsigned long timestampNow);
void handleTempSensor();
void handleCapSense();
void handleAdafruitUIOverrides();

void onOverrideFeed(AdafruitIO_Data *data);
void onWindowFeed(AdafruitIO_Data *data);
void onTapFeed(AdafruitIO_Data *data);
void onTempFeed(AdafruitIO_Data *data);


void setup() {
    display.initDisplay();
    Serial.begin(115200);
    initWifi();
    initButtons();
    initPot();
    initTempSensor();
    initCapsense();
    adafruit.init(onOverrideFeed, onWindowFeed, onTapFeed, onTempFeed); // hacky but can't figure out a better way to pass something that matches the right type
}

void loop() {
    unsigned long now = millis();

    handlePot();
    handleLightSensor(now);
    handleTempSensor();
    handleCapSense();
    adafruit.handleLoop(now);
    handleAdafruitUIOverrides();

    if (debugMode.isDebugModeEnabled()) {
        Serial.printf("targetTemp: %d\u00B0C light lvl: %d light: %d temp: % 4.2f\u00B0C waterRunning: %d \n", 
        state.getConfiguredTemperatue(), 
        (int) state.getLight(), 
        state.getLightRaw(),
        state.getTrueTemperature(), 
        (int) state.isWaterRunning());
    }

    display.updateScreenData();
    delay(200);    
}

void initButtons() {
    // buttons
    pinMode(DOOR_BTN_PIN, INPUT_PULLUP);
    pinMode(WINDOW_BTN_PIN, INPUT_PULLUP);

    attachInterrupt(DOOR_BTN_PIN, onChangeDoorButton, CHANGE);
    attachInterrupt(WINDOW_BTN_PIN, onChangeWindowButton, CHANGE);

    // initialise states
    onChangeDoorButton();
    onChangeWindowButton();
}

void initPot() {
    // potentiometer = configure temperature
    pinMode(POT_PIN, INPUT);
}

void initTempSensor() {
    bool status = bmp.begin(0x76);
    
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring and reboot!");
        while (1) delay(10);
    }
}

void initCapsense() {
    touch_pad_init();

    // "Water kraan"
    initCapsenseItem(CAPSENSE_WATERTAP_TOUCHNUM);

    // "Ruwe vochtigheidsmeting grond"
    initCapsenseItem(CAPSENSE_SOILMOISTURE_TOUCHNUM);
}

void initCapsenseItem(touch_pad_t touch_num) {
    touch_pad_config(touch_num, 0);
    touch_pad_filter_start(10);
    touch_pad_set_cnt_mode(touch_num, TOUCH_PAD_SLOPE_7, TOUCH_PAD_TIE_OPT_LOW);
    touch_pad_set_voltage(TOUCH_HVOLT_2V4, TOUCH_LVOLT_0V8, TOUCH_HVOLT_ATTEN_1V5);
}

void initWifi() {
    Serial.printf("WIFI: Connecting to %s...\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PW);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    Serial.println("WiFi connected.");

    initWifiTime();

    //disconnect WiFi as it's no longer needed
    WiFi.disconnect(true);
}

void initWifiTime() {
    // time setup
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
        return;
    }

    // we need to correct the time for the Belgian GMT+1 timezone
    timeinfo.tm_hour += TIME_CORRECTION_HOURS;
    mktime(&timeinfo);

    data.init(timeinfo, millis());
}

void onChangeDoorButton() {
    bool notPressed = digitalRead(DOOR_BTN_PIN); // notPressed == open
    state.setDoorOpen(notPressed);
    if (!notPressed) debugMode.onButtonPressed(DOOR_BTN_PIN);
}

void onChangeWindowButton() {
    bool notPressed = digitalRead(WINDOW_BTN_PIN); // notPressed == open
    state.setWindowOpen(notPressed);
    if (!notPressed) debugMode.onButtonPressed(WINDOW_BTN_PIN);
}

void handlePot() {
    if (adafruit.isOverrideEnabled()) return; // skip updates if overriding

    int potValue = analogRead(POT_PIN);
    int tempChoice = potUtil.mapPotValToTemp(potValue);
    state.setConfiguredTemperature(tempChoice);
}

void handleLightSensor(unsigned long timestampNow) {
    int lightSensorVal = analogRead(LIGHT_SENSOR_PIN);

    state.setLightRaw(lightSensorVal, ADC_MAX_VAL);
    data.onNewLightValue(timestampNow, state.getLight());
    
    if (debugMode.isDebugModeEnabled()) {
        data._debugPrintData();
    }
}

void handleTempSensor() {
    float temperature = bmp.readTemperature();
    state.setTemperature(temperature);

    if (adafruit.isOverrideEnabled()) return; // skip logic if override

    if (state.getTempState() == TEMP_STATE::MAJOR_DIFFERENCE) {
        if (state.getTrueTemperature() > state.getConfiguredTemperatue()) {
            interactivity.openWindow();
        } else {
            interactivity.closeWindow();
        }
    }
}

void handleCapSense() {
    uint16_t ReadTouchVal = 0;

    touch_pad_read_filtered(CAPSENSE_WATERTAP_TOUCHNUM, &ReadTouchVal);
    if (debugMode.isDebugModeEnabled()) Serial.printf("Water Tap Capacitance: %d\n", ReadTouchVal);
    state.updateCapsenseWaterTap(ReadTouchVal);


    touch_pad_read_filtered(CAPSENSE_SOILMOISTURE_TOUCHNUM, &ReadTouchVal);
    if (debugMode.isDebugModeEnabled()) Serial.printf("Soil moisture Capacitance: %d\n", ReadTouchVal);
    state.updateCapsenseSoilMoisture(ReadTouchVal);

    if (adafruit.isOverrideEnabled()) return; // skip logic if override

    MOISTURE_LEVEL soilMoisture = state.getSoilMoistureLevel();
    
    if (soilMoisture == MOISTURE_LEVEL::OK && state.isWaterRunning()) {
        interactivity.closeWaterTap();
    } else if (soilMoisture == MOISTURE_LEVEL::TOO_DRY) {
        interactivity.openWaterTap();
    } else if (soilMoisture == MOISTURE_LEVEL::TOO_WET) {
        interactivity.closeWaterTap();
    }
}

void handleAdafruitUIOverrides() {
    if (!adafruit.isOverrideEnabled()) return;

    if (adafruit.getOverrideOpenTap() && !state.isWaterRunning()) {

        if (debugMode.isDebugModeEnabled()) Serial.println("Adafruit.IO Overrule: Opening Water Tap...");
        interactivity.openWaterTap();

    } else if (!adafruit.getOverrideOpenTap() && state.isWaterRunning()) {

        if (debugMode.isDebugModeEnabled()) Serial.println("Adafruit.IO Overrule: Closing Water Tap...");
        interactivity.closeWaterTap();
    }

    if (adafruit.getOverrideOpenWindow() && !state.isWindowOpen()) {
        
        if (debugMode.isDebugModeEnabled()) Serial.println("Adafruit.IO Overrule: Opening Window...");
        interactivity.openWindow();

    } else if (!adafruit.getOverrideOpenWindow() && state.isWindowOpen()) {

        if (debugMode.isDebugModeEnabled()) Serial.println("Adafruit.IO Overrule: Opening Window...");
        interactivity.closeWindow();
    }

    int conversionVal = potUtil.convertToWithinLimit(adafruit.getOverrideConfigureTemp());
    if (debugMode.isDebugModeEnabled()) Serial.printf("Adafruit.IO Overrule: Setting temperature target to %d\u00B0C...\n", conversionVal);
    state.setConfiguredTemperature(conversionVal);
}

void onOverrideFeed(AdafruitIO_Data *data) {
    adafruit.handleOverrideEnableFeed(data);
}

void onWindowFeed(AdafruitIO_Data *data) {
    adafruit.handleOverrideWindowFeed(data);
}

void onTapFeed(AdafruitIO_Data *data) {
    adafruit.handleOverrideTapFeed(data);
}

void onTempFeed(AdafruitIO_Data *data) {
    adafruit.handleOverrideTempFeed(data);
}
