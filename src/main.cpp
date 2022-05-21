#include <Arduino.h>

#include "StateManager.h"
#include "DisplayManager.h"
#include "PotTemperatureUtil.h"

#define DOOR_BTN_PIN 0
#define WINDOW_BTN_PIN 35
#define POT_PIN 27
#define LIGHT_SENSOR_PIN 2

#define ADC_MAX_VAL 4095
#define DEBOUNCE_MS 200


StateManager state = StateManager();
DisplayManager display = DisplayManager(state);
PotTemperatureUtil potUtil = PotTemperatureUtil((float) ADC_MAX_VAL);


unsigned long lastClickDoorBtn = 0;
unsigned long lastClickWindowBtn = 0;


void initButtons();
void initPot();

void onClickDoorButton();
void onClickWindowButton();

void handlePot();
void handleLightSensor();

void setup() {
    display.initDisplay();
    Serial.begin(115200);
    initButtons();
}

void loop() {
    handlePot();
    handleLightSensor();

    Serial.printf("Hello. temp: %d\u00B0C   lightVal: %d\n", state.getConfiguredTemperatue(), (int) state.getLight());
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
