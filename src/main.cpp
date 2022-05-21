#include <Arduino.h>

#include "StateManager.h"
#include "DisplayManager.h"
#include "PotTemperatureUtil.h"

#define DOOR_BTN_PIN 0
#define WINDOW_BTN_PIN 35
#define POT_PIN 27

#define POT_MAX_VAL 4095.0

StateManager state = StateManager();
DisplayManager display = DisplayManager(state);
PotTemperatureUtil potUtil = PotTemperatureUtil(POT_MAX_VAL);

void initButtons();
void initPot();

void onClickDoorButton();
void onClickWindowButton();

void handlePot();

void setup() {
    display.initDisplay();
    Serial.begin(115200);
    initButtons();
}

void loop() {
    handlePot();

    Serial.printf("Hello. temp: %d\u00B0C \n", state.getConfiguredTemperatue());
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

void onClickDoorButton() {
    // TODO: debounce
    state.setDoorOpen(!state.isDoorOpen());
}

void onClickWindowButton() {
    // TODO: debounce
    state.setWindowOpen(!state.isWindowOpen());
}

void handlePot() {
    int potValue = analogRead(POT_PIN);
    int tempChoice = potUtil.mapPotValToTemp(potValue);
    state.setConfiguredTemperature(tempChoice);
}

