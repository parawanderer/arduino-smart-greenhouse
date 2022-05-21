#include "DisplayManager.h"

// resolution
// 135X240

#define DOOR_ICON_X 0
#define DOOR_ICON_Y 0

#define WINDOW_ICON_X 25
#define WINDOW_ICON_Y 0

DisplayManager::DisplayManager(StateManager& stateManager): m_stateManager(stateManager) {
}

void DisplayManager::initDisplay() {
    this->m_tft.init();
    this->m_displayWidth = this->m_tft.width();
    this->m_displayHeight = this->m_tft.height();

    this->m_tft.setRotation(1);   // setRotation: 1: Screen in landscape
    this->m_tft.fillScreen(TFT_BLACK);
    // this->m_tft.setCursor(30, 0, 4);   //(cursor at 0,0; font 4, println autosets the cursor on the next line)
    // this->m_tft.setTextColor(TFT_BLACK, TFT_YELLOW); // Textcolor, BackgroundColor; independent of the fillscreen
    // this->m_tft.println("- Lab 3 - ShiftReg -");    //Print on cursorpos 0,0
}

void DisplayManager::updateScreenData() {

    if (this->m_stateManager.isDoorOpen()) {
        this->m_tft.drawXBitmap(DOOR_ICON_X, DOOR_ICON_Y, DOOR_OPEN_ICON_BITS, DOOR_OPEN_ICON_WIDTH, DOOR_OPEN_ICON_HEIGHT, TFT_WHITE, TFT_BLACK);
    } else {
        this->m_tft.drawXBitmap(DOOR_ICON_X, DOOR_ICON_Y, DOOR_CLOSED_ICON_BITS, DOOR_CLOSED_ICON_WIDTH, DOOR_CLOSED_ICON_HEIGHT, TFT_DARKGREY, TFT_BLACK);
    }

    if (this->m_stateManager.isWindowOpen()) {
        this->m_tft.drawXBitmap(WINDOW_ICON_X, WINDOW_ICON_Y, WINDOW_OPEN_ICON_BITS, WINDOW_OPEN_ICON_WIDTH, WINDOW_OPEN_ICON_HEIGHT, TFT_WHITE, TFT_BLACK);
    } else {
        this->m_tft.drawXBitmap(WINDOW_ICON_X, WINDOW_ICON_Y, WINDOW_CLOSED_ICON_BITS, WINDOW_CLOSED_ICON_WIDTH, WINDOW_CLOSED_ICON_HEIGHT, TFT_DARKGREY, TFT_BLACK);
    }
}