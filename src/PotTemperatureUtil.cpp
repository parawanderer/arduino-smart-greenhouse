#include "PotTemperatureUtil.h"

PotTemperatureUtil::PotTemperatureUtil(float maxVal) : m_potMaxVal(maxVal), m_tempInterval(MAX_TEMP - MIN_TEMP) {
}

int PotTemperatureUtil::mapPotValToTemp(int potVal) {
    return (int) (((potVal / this->m_potMaxVal) * this->m_tempInterval) + MIN_TEMP);
}