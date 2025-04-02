#ifndef MQ4_MANAGER_H
#define MQ4_MANAGER_H

#include <Arduino.h>

#define MQ4_PIN 32  // Pin donde está conectado el MQ-4

class MQ4Manager {
public:
    MQ4Manager();
    void begin();
    void update();
    float calculatePPM(int rawValue); // Función para calcular PPM

    // Getters para acceder a los valores actuales
    int getRawValue() const { return rawValue; }
    float getVoltage() const { return voltage; }
    float getPPM() const { return ppmCH4; }

private:
    int rawValue;
    float voltage;
    float ppmCH4;  // Concentración estimada de metano
};

#endif