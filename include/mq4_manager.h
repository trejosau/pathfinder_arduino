#ifndef MQ4_MANAGER_H
#define MQ4_MANAGER_H

#include <Arduino.h>

#define MQ4_PIN 35  // Pin donde está conectado el MQ-4

class MQ4Manager {
public:
    MQ4Manager();
    void begin();
    void update();
    float calculatePPM(int rawValue); // Declaración de la función

private:
    int rawValue;
    float voltage;
    float ppmCH4;  // Concentración estimada de metano
};

#endif
