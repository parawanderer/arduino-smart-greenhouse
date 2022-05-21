#include "DisplayManager.h"

// resolution
// 135X240

#define DOOR_ICON_SHARED_WIDTH DOOR_OPEN_ICON_WIDTH
#define DOOR_ICON_SHARED_HEIGHT DOOR_OPEN_ICON_HEIGHT

#define WINDOW_ICON_SHARED_WIDTH WINDOW_OPEN_ICON_WIDTH
#define WINDOW_ICON_SHARED_HEIGHT WINDOW_OPEN_ICON_HEIGHT

#define LIGHT_ICON_SHARED_WIDTH SUN_ICON_1_WIDTH
#define LIGHT_ICON_SHARED_HEIGHT SUN_ICON_1_HEIGHT

#define DOOR_ICON_X 0
#define DOOR_ICON_Y 0

#define WINDOW_ICON_X DOOR_OPEN_ICON_WIDTH + 7
#define WINDOW_ICON_Y 0

#define LIGHT_ICON_X WINDOW_ICON_X + WINDOW_ICON_SHARED_WIDTH + 7
#define LIGHT_ICON_Y 0

// sun icons map
const unsigned char* SUN_ICONS[] = {
    SUN_ICON_6_BITS,
    SUN_ICON_5_BITS,
    SUN_ICON_4_BITS,
    SUN_ICON_3_BITS,
    SUN_ICON_2_BITS,
    SUN_ICON_1_BITS
};



DisplayManager::DisplayManager(StateManager& stateManager): m_stateManager(stateManager) {
}

void DisplayManager::initDisplay() {
    this->m_tft.init();

    this->m_tft.setRotation(1);   // setRotation: 1: Screen in landscape
    this->m_tft.fillScreen(TFT_BLACK);

    this->m_displayWidth = this->m_tft.width();
    this->m_displayHeight = this->m_tft.height();
    // this->m_tft.setCursor(30, 0, 4);   //(cursor at 0,0; font 4, println autosets the cursor on the next line)
    // this->m_tft.setTextColor(TFT_BLACK, TFT_YELLOW); // Textcolor, BackgroundColor; independent of the fillscreen
    // this->m_tft.println("- Lab 3 - ShiftReg -");    //Print on cursorpos 0,0
}

void DisplayManager::updateScreenData() {
    // door
    const unsigned char* ICON = this->m_stateManager.isDoorOpen() ? DOOR_OPEN_ICON_BITS : DOOR_CLOSED_ICON_BITS;
    this->m_tft.drawXBitmap(DOOR_ICON_X, DOOR_ICON_Y, ICON, DOOR_ICON_SHARED_WIDTH, DOOR_ICON_SHARED_HEIGHT, TFT_WHITE, TFT_BLACK);


    // window
    ICON = this->m_stateManager.isWindowOpen() ? WINDOW_OPEN_ICON_BITS : WINDOW_CLOSED_ICON_BITS;
    this->m_tft.drawXBitmap(WINDOW_ICON_X, WINDOW_ICON_Y, ICON, WINDOW_ICON_SHARED_WIDTH, WINDOW_ICON_SHARED_HEIGHT, TFT_WHITE, TFT_BLACK);


    // light level
    int lightLevel = (int) this->m_stateManager.getLight();
    this->m_tft.drawXBitmap(
        LIGHT_ICON_X, 
        LIGHT_ICON_Y, 
        SUN_ICONS[lightLevel], 
        LIGHT_ICON_SHARED_WIDTH, 
        LIGHT_ICON_SHARED_HEIGHT, 
        TFT_WHITE, 
        TFT_BLACK);


    // temp (targetted)
    int offsetTextY = this->m_displayHeight - 55;
    this->m_tft.setCursor(5, offsetTextY, 2);
    this->m_tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    this->m_tft.setTextSize(1);
    int configuredTemp = this->m_stateManager.getConfiguredTemperatue();
    this->m_tft.printf("TARGET: %d C  ", configuredTemp);
    int offset = configuredTemp > 9 ? 80 : 73;
    this->m_tft.drawCircle(offset, offsetTextY + 3, 2, TFT_YELLOW); // there is unfortunately no support for the degree icon so we will draw a circle instead


    // temp (actual)
    offsetTextY = this->m_displayHeight - 30;
    this->m_tft.setCursor(5, offsetTextY, 4);
    StateManager::TEMP_STATE tempState = this->m_stateManager.getTempState();

    int color = tempState == StateManager::TEMP_STATE::OK ? TFT_WHITE : TFT_YELLOW;
    if (tempState == StateManager::TEMP_STATE::MAJOR_DIFFERENCE) {
        color = TFT_RED;
    }

    this->m_tft.setTextColor(color, TFT_BLACK);
    this->m_tft.setTextSize(1);
    float trueTemp = this->m_stateManager.getTrueTemperature();
    this->m_tft.printf("% 4.2f  C", trueTemp);
    this->m_tft.drawCircle(78, offsetTextY + 4, 4, color); // there is unfortunately no support for the degree icon so we will draw a circle instead

    if (tempState == StateManager::TEMP_STATE::MAJOR_DIFFERENCE) {
        this->m_tft.drawXBitmap(105, offsetTextY + 1, DANGER_ICON_BITS, DANGER_ICON_WIDTH, DANGER_ICON_HEIGHT, TFT_RED, TFT_BLACK);
    } else {
        this->m_tft.fillRect(105, offsetTextY + 1, DANGER_ICON_WIDTH, DANGER_ICON_HEIGHT, TFT_BLACK);
    }
}