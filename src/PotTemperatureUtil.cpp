#include "PotTemperatureUtil.h"

PotTemperatureUtil::PotTemperatureUtil(float maxVal) : m_potMaxVal(maxVal), m_tempInterval(MAX_TEMP - MIN_TEMP) {
}

int PotTemperatureUtil::mapPotValToTemp(int potVal) {
    return (int) (((potVal / this->m_potMaxVal) * this->m_tempInterval) + MIN_TEMP);
}

int PotTemperatureUtil::convertToWithinLimit(int val) {
    if (val < MIN_TEMP) return MIN_TEMP;
    if (val > MAX_TEMP) MAX_TEMP;
    return val;
}