#ifndef TOKYODRIFT_H
#define TOKYODRIFT_H

#include <Arduino.h>

class TokyoDrift {
public:
    // Constructor: recibe el pin del buzzer
    TokyoDrift(uint8_t buzzerPin);

    // Función que reproduce la melodía "Tokyo Drift"
    void play();

private:
    uint8_t _buzzerPin;
};

#endif
