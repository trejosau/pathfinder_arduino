#ifndef MQ7_MANAGER_H
#define MQ7_MANAGER_H

#include <Arduino.h>

#define MQ7_PIN 33

class MQ7Manager {
public:
    MQ7Manager();
    void begin();
    void update();

    // Getters para acceder a los valores actuales
    int getRawValue() const { return rawValue; }
    float getVoltage() const { return voltage; }
    float getPPM() const { return ppmCO; }

private:
    int rawValue;
    float voltage;
    float ppmCO;  // Concentración estimada de monóxido de carbono
};

#endif