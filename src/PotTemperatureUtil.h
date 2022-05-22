#ifndef POTTEMPERATUREUTIL_H
#define POTTEMPERATUREUTIL_H

// https://www.houseplantsexpert.com/indoor-plants-temperature-guide.html#:~:text=Temperature%20too%20high%3A%20Above%2075,growth%20without%20confining%20the%20heat.
#define MAX_TEMP  30.0  // 27.0
#define MIN_TEMP 7.0

class PotTemperatureUtil {
    public:
        PotTemperatureUtil(float maxVal);

        int mapPotValToTemp(int potVal);
    private:
        float m_potMaxVal;
        float m_tempInterval;
};

#endif