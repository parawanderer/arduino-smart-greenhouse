#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#include "StateManager.h"
#include "DisplayManager.h"
#include "PotTemperatureUtil.h"

#define DOOR_BTN_PIN 0
#define WINDOW_BTN_PIN 35
#define POT_PIN 27
#define LIGHT_SENSOR_PIN 2

#define ADC_MAX_VAL 4095
#define DEBOUNCE_MS 200

// for Adafruit_BMP280
#define SEALEVELPRESSURE_HPA (1013.25)

StateManager state = StateManager();
DisplayManager display = DisplayManager(state);
PotTemperatureUtil potUtil = PotTemperatureUtil((float) ADC_MAX_VAL);


Adafruit_BMP280 bmp = Adafruit_BMP280();


unsigned long lastClickDoorBtn = 0;
unsigned long lastClickWindowBtn = 0;

void initButtons();
void initPot();
void initTempSensor();

void onClickDoorButton();
void onClickWindowButton();

void handlePot();
void handleLightSensor();
void handleTempSensor();

void setup() {
    display.initDisplay();
    Serial.begin(115200);
    initButtons();
    initPot();
    initTempSensor();
}

void loop() {
    handlePot();
    handleLightSensor();
    handleTempSensor();

    Serial.printf("Hello. targetTemp: %d\u00B0C  light: %d  temp: % 4.2f\u00B0C\n", state.getConfiguredTemperatue(), (int) state.getLight(), state.getTrueTemperature());
    display.updateScreenData();
    delay(100);    
}

void initButtons() {
    // buttons
    pinMode(DOOR_BTN_PIN, INPUT_PULLUP);
    pinMode(WINDOW_BTN_PIN, INPUT_PULLUP);

    attachInterrupt(DOOR_BTN_PIN, onClickDoorButton, RISING);
    attachInterrupt(WINDOW_BTN_PIN, onClickWindowButton, RISING);
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

void handleDebounced(unsigned long* lastClickTime, void (*onAllowClick)()) {
    // ik weet dat het niet fantastisch is om functies zoals millis te gebruiken in interrupt functies
    unsigned long now = millis();
    
    // handle overflow
    if (now < *lastClickTime) {
        *lastClickTime = LONG_MAX - *lastClickTime; 
    }

    if (now - *lastClickTime > DEBOUNCE_MS) {
        // allow clicking
        onAllowClick();
        *lastClickTime = now;
    }
}

void onClickDoorButton() {
    handleDebounced(&lastClickDoorBtn, []() {
        state.setDoorOpen(!state.isDoorOpen());
    });
}

void onClickWindowButton() {
    handleDebounced(&lastClickWindowBtn, []() {
        state.setWindowOpen(!state.isWindowOpen());
    });
}

void handlePot() {
    int potValue = analogRead(POT_PIN);
    int tempChoice = potUtil.mapPotValToTemp(potValue);
    state.setConfiguredTemperature(tempChoice);
}

void handleLightSensor() {
    int lightSensorVal = analogRead(LIGHT_SENSOR_PIN);
    state.setLightRaw(lightSensorVal, ADC_MAX_VAL);
}

void handleTempSensor() {
    float temperature = bmp.readTemperature();
    state.setTemperature(temperature);
}