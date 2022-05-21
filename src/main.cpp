#include <Arduino.h>

#include "StateManager.h"
#include "DisplayManager.h"

#define DOOR_BTN_PIN 0
#define WINDOW_BTN_PIN 35

StateManager state = StateManager();
DisplayManager display = DisplayManager(state);

void initButtons();
void initDisplay();

void onClickDoorButton();
void onClickWindowButton();

void setup() {
    display.initDisplay();
    Serial.begin(115200);
    initButtons();
}

void loop() {
    Serial.printf("Hello. Door Open: %d   Window Open: %d\n", state.isDoorOpen(), state.isWindowOpen());
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

void onClickDoorButton() {
    state.setDoorOpen(!state.isDoorOpen());
}

void onClickWindowButton() {
    state.setWindowOpen(!state.isWindowOpen());
}