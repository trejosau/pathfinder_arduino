#ifndef INCLINE_SENSOR_H
#define INCLINE_SENSOR_H

#include <Arduino.h>

class InclineSensor {
private:
    uint8_t pin;  // Pin donde se conecta DO
public:
    InclineSensor(uint8_t pin);
    void begin();
    // Lee el sensor y retorna 'true' si detecta inclinación (por ejemplo, se cierra el contacto) y 'false' en caso contrario.
    bool isInclined();
};

#endif
