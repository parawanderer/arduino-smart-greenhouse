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
#include "DebugModeManager.h"

#include "PotTemperatureUtil.h"

#define DOOR_BTN_PIN 0
#define WINDOW_BTN_PIN 35
#define POT_PIN 32
#define LIGHT_SENSOR_PIN 2

#define ADC_MAX_VAL 4095
#define DEBOUNCE_MS 200

// for Adafruit_BMP280
#define SEALEVELPRESSURE_HPA (1013.25)


// time server setup
#define NTP_SERVER "0.be.pool.ntp.org"
#define GMT_OFFSET_SEC 0
#define DAYLIGHT_OFFSET_SEC 3600
#define TIME_CORRECTION_HOURS 1 // we need GMT + 1 for belgium


StateManager state = StateManager();
InteractivityManager interactivity = InteractivityManager(state);
DataManager data = DataManager(io);
DisplayManager display = DisplayManager(state, data);
DebugModeManager debugMode = DebugModeManager(DOOR_BTN_PIN, WINDOW_BTN_PIN);

PotTemperatureUtil potUtil = PotTemperatureUtil((float) ADC_MAX_VAL);


Adafruit_BMP280 bmp = Adafruit_BMP280();


void initButtons();
void initPot();
void initTempSensor();
void initCapsense();
void initWifi();
void initWifiTime();
void initAdafruitIO();

void onChangeDoorButton();
void onChangeWindowButton();

void handlePot();
void handleLightSensor(unsigned long timestampNow);
void handleTempSensor();
void handleCapSense(unsigned long timestampNow);
void handleAdafruitIO();

void setup() {
    display.initDisplay();
    Serial.begin(115200);
    initWifi();
    initButtons();
    initPot();
    initTempSensor();
    initCapsense();
}

void loop() {
    unsigned long now = millis();

    handlePot();
    handleLightSensor(now);
    handleTempSensor();
    handleCapSense(now);

    Serial.printf("targetTemp: %d\u00B0C  light: %d  temp: % 4.2f\u00B0C  waterRunning: %d \n", state.getConfiguredTemperatue(), (int) state.getLight(), state.getTrueTemperature(), (int) state.isWaterRunning());
    display.updateScreenData();
    delay(100);    
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
    touch_pad_config(TOUCH_PAD_NUM7, 0);
    touch_pad_filter_start(10);
    touch_pad_set_cnt_mode(TOUCH_PAD_NUM7, TOUCH_PAD_SLOPE_7, TOUCH_PAD_TIE_OPT_LOW);
    touch_pad_set_voltage(TOUCH_HVOLT_2V4, TOUCH_LVOLT_0V8, TOUCH_HVOLT_ATTEN_1V5);

    // TODO: "Ruwe vochtigheidsmeting grond"
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
    WiFi.mode(WIFI_OFF);
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

void initAdafruitIO() {
    Serial.printf("WIFI - ADAFRUIT.IO: Connecting as %s...\n", IO_USERNAME);
    io.connect();

    while (io.status() < AIO_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    Serial.println(io.statusText());

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
    int potValue = analogRead(POT_PIN);
    int tempChoice = potUtil.mapPotValToTemp(potValue);
    state.setConfiguredTemperature(tempChoice);
}

void handleLightSensor(unsigned long timestampNow) {
    int lightSensorVal = analogRead(LIGHT_SENSOR_PIN);

    state.setLightRaw(lightSensorVal, ADC_MAX_VAL);
    data.onNewLightValue(timestampNow, state.getLight());

    if (debugMode.isDebugModeEnabled()) data._debugPrintData();
}

void handleTempSensor() {
    float temperature = bmp.readTemperature();
    state.setTemperature(temperature);

    if (state.getTempState() == TEMP_STATE::MAJOR_DIFFERENCE) {
        if (state.getTrueTemperature() > state.getConfiguredTemperatue()) {
            interactivity.openWindow();
        } else {
            interactivity.closeWindow();
        }
    }
}

void handleCapSense(unsigned long timestampNow) {

    uint16_t ReadTouchVal = 0;

    // touch_pad_read(TOUCH_PAD_NUM7, &ReadTouchVal);
    // Serial.print(ReadTouchVal);Serial.write(',');

    //Lees hier de gefilterde touch data
    touch_pad_read_filtered(TOUCH_PAD_NUM7, &ReadTouchVal);

    state.updateCapsenseWaterTap(ReadTouchVal, timestampNow);

    // TODO: soil moisture level
    bool soilMoistureTooWet = false;
    bool soilMoistureTooDry = false;
    bool soilMoistureOk = true;
    
    if (soilMoistureOk && state.isWaterRunning()) {
        interactivity.closeWaterTap();
    } else if (soilMoistureTooDry) {
        interactivity.openWaterTap();
    } else if (soilMoistureTooWet) {
        interactivity.closeWaterTap();
    }
}

void handleAdafruitIO() {
    // io.run(); is required for all sketches.
    // it should always be present at the top of your loop
    // function. it keeps the client connected to
    // io.adafruit.com, and processes any incoming data.
    io.run();
}