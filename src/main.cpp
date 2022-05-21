#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#include <driver\touch_pad.h>

#include "StateManager.h"
#include "DisplayManager.h"
#include "PotTemperatureUtil.h"

#define DOOR_BTN_PIN 0
#define WINDOW_BTN_PIN 35
#define POT_PIN 32
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
void initCapsense();

void onChangeDoorButton();
void onChangeWindowButton();

void handlePot();
void handleLightSensor();
void handleTempSensor();
void handleCapSense(unsigned long timestampNow);

void setup() {
    display.initDisplay();
    Serial.begin(115200);
    initButtons();
    initPot();
    initTempSensor();
    initCapsense();
}

void loop() {
    unsigned long now = millis();

    handlePot();
    handleLightSensor();
    handleTempSensor();
    handleCapSense(now);

    Serial.printf("targetTemp: %d\u00B0C  light: %d  temp: % 4.2f\u00B0C  waterRunning: %d \n", state.getConfiguredTemperatue(), (int) state.getLight(), state.getTrueTemperature(), (int) state.isWaterRunning());
    display.updateScreenData();
    delay(100);    
}

void initButtons() {
    // buttons
    pinMode(DOOR_BTN_PIN, INPUT);
    pinMode(WINDOW_BTN_PIN, INPUT);

    attachInterrupt(DOOR_BTN_PIN, onChangeDoorButton, CHANGE);
    attachInterrupt(WINDOW_BTN_PIN, onChangeWindowButton, CHANGE);
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

void onChangeDoorButton() {
    bool notPressed = digitalRead(DOOR_BTN_PIN); // notPressed == open
    state.setDoorOpen(notPressed);
}

void onChangeWindowButton() {
    bool notPressed = digitalRead(WINDOW_BTN_PIN); // notPressed == open
    state.setWindowOpen(notPressed);
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

void handleCapSense(unsigned long timestampNow) {
    // TODO: needs to be updated to 1M Ohm resistor IRL
    // long total = capSensor.capacitiveSensor(CAPSENSE_SAMPLES_NR);
    // Serial.printf("Capsense: %d\n", total);

    uint16_t ReadTouchVal = 0;

    // touch_pad_read(TOUCH_PAD_NUM7, &ReadTouchVal);
    // Serial.print(ReadTouchVal);Serial.write(',');

    //Lees hier de gefilterde touch data
    touch_pad_read_filtered(TOUCH_PAD_NUM7, &ReadTouchVal);
    //Serial.println(ReadTouchVal);

    Serial.printf("touch val: %d\n", ReadTouchVal);
    state.updateCapsenseWaterTap(ReadTouchVal, timestampNow);
}