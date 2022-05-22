#include "DisplayManager.h"

// resolution
// 135X240

#define DOOR_ICON_SHARED_WIDTH DOOR_OPEN_ICON_WIDTH
#define DOOR_ICON_SHARED_HEIGHT DOOR_OPEN_ICON_HEIGHT

#define WINDOW_ICON_SHARED_WIDTH WINDOW_OPEN_ICON_WIDTH
#define WINDOW_ICON_SHARED_HEIGHT WINDOW_OPEN_ICON_HEIGHT

#define LIGHT_ICON_SHARED_WIDTH SUN_ICON_1_WIDTH
#define LIGHT_ICON_SHARED_HEIGHT SUN_ICON_1_HEIGHT

#define WATER_ICON_SHARED_WIDTH WATER_FLOWING_ICON_WIDTH
#define WATER_ICON_SHARED_HEIGHT WATER_FLOWING_ICON_HEIGHT

#define DOOR_ICON_X 0
#define DOOR_ICON_Y 0

#define WINDOW_ICON_X DOOR_OPEN_ICON_WIDTH + 7
#define WINDOW_ICON_Y 0

#define LIGHT_ICON_X WINDOW_ICON_X + WINDOW_ICON_SHARED_WIDTH + 7
#define LIGHT_ICON_Y 0

#define WATER_ICON_X LIGHT_ICON_X + LIGHT_ICON_SHARED_WIDTH + 7
#define WATER_ICON_Y 0

#define LIGHT_ICON_SMALL_SHARED_WIDTH LIGHT_HRS_0_WIDTH
#define LIGHT_ICON_SMALL_SHARED_HEIGHT LIGHT_HRS_0_HEIGHT


#define LIGHT_HISTORY_ITEM_WIDTH   30
#define LIGHT_HISTORY_ITEMS_WIDTH  LIGHT_HISTORY_ITEM_WIDTH * 3
#define UNSET_LIGHT_VAL (byte)-1


#define COLOR_SAND  (uint16_t) (252 << (5 + 6)) | (238 << 5) | 184  /* 252, 238, 184 */
#define COLOR_DRY_DIRT  (uint16_t) (227 << (5 + 6)) | (163 << 5) | 59  /* 227, 163, 59 */
#define COLOR_LIGHTBLUE  (uint16_t) TFT_SKYBLUE  
#define COLOR_WET_BLUE  (uint16_t) (45 << (5 + 6)) | (87 << 5) | 214  /* 45, 87, 214 */

const unsigned char* SUN_ICONS[] = {
    SUN_ICON_6_BITS,
    SUN_ICON_5_BITS,
    SUN_ICON_4_BITS,
    SUN_ICON_3_BITS,
    SUN_ICON_2_BITS,
    SUN_ICON_1_BITS
};

const unsigned char* LIGHT_ICONS_SM[] = {
    LIGHT_HRS_0_BITS,
    LIGHT_HRS_1_BITS,
    LIGHT_HRS_2_BITS,
    LIGHT_HRS_3_BITS,
    LIGHT_HRS_4_BITS,
    LIGHT_HRS_5_BITS
};

const char* MOISTURE_LVLS_TEXT[] = {
    "TOO DRY!",
    "Dry",
    "Ok",
    "Wet",
    "TOO WET!"
};

uint16_t MOISTURE_LVLS_COLORS[] = {
    COLOR_DRY_DIRT,
    COLOR_SAND,
    TFT_WHITE,
    COLOR_LIGHTBLUE,
    COLOR_WET_BLUE,
};


DisplayManager::DisplayManager(StateManager& stateManager, DataManager& dataManager): m_stateManager(stateManager), m_dataManager(dataManager) {
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
    const unsigned char* ICON;

    // door
    ICON = this->m_stateManager.isDoorOpen() ? DOOR_OPEN_ICON_BITS : DOOR_CLOSED_ICON_BITS;
    this->m_tft.drawXBitmap(DOOR_ICON_X, DOOR_ICON_Y, ICON, DOOR_ICON_SHARED_WIDTH, DOOR_ICON_SHARED_HEIGHT, TFT_WHITE, TFT_BLACK);


    // window
    ICON = this->m_stateManager.isWindowOpen() ? WINDOW_OPEN_ICON_BITS : WINDOW_CLOSED_ICON_BITS;
    this->m_tft.drawXBitmap(WINDOW_ICON_X, WINDOW_ICON_Y, ICON, WINDOW_ICON_SHARED_WIDTH, WINDOW_ICON_SHARED_HEIGHT, TFT_WHITE, TFT_BLACK);


    // light level
    int lightLevel = (int) this->m_stateManager.getLight();
    this->m_tft.drawXBitmap(LIGHT_ICON_X, LIGHT_ICON_Y, SUN_ICONS[lightLevel], LIGHT_ICON_SHARED_WIDTH, LIGHT_ICON_SHARED_HEIGHT, TFT_WHITE, TFT_BLACK);

    this->drawWaterFlowing();
    this->drawTargetTemp();
    this->drawTrueTemp();
    this->drawSoilMoisture();
    this->drawDaylightHrsHistory();
}

void DisplayManager::drawWaterFlowing() {
    // water flowing
    const unsigned char* ICON = WATER_NOFLOW_ICON_BITS;
    int color = TFT_WHITE;

    if (this->m_stateManager.isWaterRunning()) {
        ICON = WATER_FLOWING_ICON_BITS;
        color = TFT_SKYBLUE;
    }
    
    this->m_tft.drawXBitmap(WATER_ICON_X, WATER_ICON_Y, ICON, WATER_ICON_SHARED_WIDTH, WATER_ICON_SHARED_HEIGHT, color, TFT_BLACK);
}

void DisplayManager::drawTargetTemp() {
    // temp (targetted)
    int offsetTextY = this->m_displayHeight - 50;
    this->m_tft.setCursor(5, offsetTextY, 2);
    this->m_tft.setTextColor(TFT_YELLOW, TFT_BLACK);

    int configuredTemp = this->m_stateManager.getConfiguredTemperatue();
    this->m_tft.printf("TARGET: %d C  ", configuredTemp);

    int offset = configuredTemp > 9 ? 80 : 73;
    this->m_tft.drawCircle(offset, offsetTextY + 3, 2, TFT_YELLOW); // there is unfortunately no support for the degree icon so we will draw a circle instead
}

void DisplayManager::drawTrueTemp() {
    // temp (actual)
    int offsetTextY = this->m_displayHeight - 25;
    this->m_tft.setCursor(5, offsetTextY, 4);
    TEMP_STATE tempState = this->m_stateManager.getTempState();

    int color = tempState == TEMP_STATE::OK ? TFT_WHITE : TFT_YELLOW;
    if (tempState == TEMP_STATE::MAJOR_DIFFERENCE) {
        color = TFT_RED;
    }

    this->m_tft.setTextColor(color, TFT_BLACK);
    float trueTemp = this->m_stateManager.getTrueTemperature();
    this->m_tft.printf("% 4.2f  C", trueTemp);
    this->m_tft.drawCircle(78, offsetTextY + 4, 4, color); // there is unfortunately no support for the degree icon so we will draw a circle instead

    if (tempState == TEMP_STATE::MAJOR_DIFFERENCE) {
        this->m_tft.drawXBitmap(105, offsetTextY + 1, DANGER_ICON_BITS, DANGER_ICON_WIDTH, DANGER_ICON_HEIGHT, TFT_RED, TFT_BLACK);
    } else {
        this->m_tft.fillRect(105, offsetTextY + 1, DANGER_ICON_WIDTH, DANGER_ICON_HEIGHT, TFT_BLACK);
    }
}

void DisplayManager::drawSoilMoisture() {
    // soil moisture level
    int offsetTextY = this->m_displayHeight - 70;
    this->m_tft.setCursor(5, offsetTextY, 2);

    int soilMoisture = (int) this->m_stateManager.getSoilMoistureLevel();

    this->m_tft.setTextColor(MOISTURE_LVLS_COLORS[soilMoisture], TFT_BLACK);
    this->m_tft.printf("Soil: %s        ", MOISTURE_LVLS_TEXT[soilMoisture]);
}

void DisplayManager::drawDaylightHrsHistory() {
    int offsetTextY = this->m_displayHeight - 50;
    int offsetTextX = this->m_displayWidth - 70;

    this->m_tft.setCursor(offsetTextX, offsetTextY, 1);
    this->m_tft.setTextColor(TFT_GOLD, TFT_BLACK);
    this->m_tft.print("Light Hrs:");

    offsetTextY += 15;

    HistoryData history = this->m_dataManager.getHistoryLast3Days();

    if (history.m_val1 == UNSET_LIGHT_VAL) {
        this->drawNoHistory(offsetTextX, offsetTextY);
        return;
    }

    // determine correct start offset for icons
    offsetTextX = this->eraseOldHistoryItems(history.m_val2, history.m_val3, offsetTextX, offsetTextY);

    // draw first history item
    this->drawHistoryItem(history.m_weekDay1, history.m_val1, offsetTextX, offsetTextY);

    if (history.m_val2 == UNSET_LIGHT_VAL) return;
    offsetTextX += LIGHT_HISTORY_ITEM_WIDTH;

    // draw second history item
    this->drawHistoryItem(history.m_weekDay2, history.m_val2, offsetTextX, offsetTextY);

    if (history.m_val3 == UNSET_LIGHT_VAL) return;
    offsetTextX += LIGHT_HISTORY_ITEM_WIDTH;

    // draw third history item
    this->drawHistoryItem(history.m_weekDay3, history.m_val3, offsetTextX, offsetTextY);
}

int DisplayManager::eraseOldHistoryItems(byte historyVal2, byte historyVal3, int offsetTextX, int offsetTextY) {
    // determine correct start offset for icons
    offsetTextX = this->m_displayWidth - LIGHT_HISTORY_ITEMS_WIDTH;
    int newOffsetTextX = offsetTextX;

    if (historyVal2 == UNSET_LIGHT_VAL) newOffsetTextX = this->m_displayWidth - LIGHT_HISTORY_ITEM_WIDTH;
    else if (historyVal3 == UNSET_LIGHT_VAL)  newOffsetTextX = this->m_displayWidth - (LIGHT_HISTORY_ITEM_WIDTH * 2);
    
    if (offsetTextX != newOffsetTextX) {
        // & erase anything old
        this->m_tft.fillRect(offsetTextX, offsetTextY, newOffsetTextX - offsetTextX, this->m_displayHeight - offsetTextY, TFT_BLACK);
    }

    return newOffsetTextX;
}

void DisplayManager::drawHistoryItem(const char* weekday, byte lightVal, int offsetX, int offsetY) {
    int lightImageIndex = (int) lightVal;
    if (lightVal < 0 || lightVal > 6) lightVal = 0;

    this->m_tft.setCursor(offsetX, offsetY + LIGHT_ICON_SMALL_SHARED_HEIGHT, 2);
    this->m_tft.setTextColor(TFT_WHITE, TFT_BLACK);
    this->m_tft.print(weekday);
    this->m_tft.drawXBitmap(offsetX + 2, offsetY, LIGHT_ICONS_SM[lightImageIndex], LIGHT_ICON_SMALL_SHARED_WIDTH, LIGHT_ICON_SMALL_SHARED_HEIGHT, TFT_WHITE, TFT_BLACK);
}

void DisplayManager::drawNoHistory(int offsetX, int offsetY) {
    // no history
    this->m_tft.setCursor(offsetX, offsetY, 1);
    this->m_tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    this->m_tft.print("No History");
}