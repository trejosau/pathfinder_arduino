#ifndef DHT_MANAGER_H
#define DHT_MANAGER_H

#include <Arduino.h>
#include <DHT.h>

class DHTManager {
public:
    DHTManager(uint8_t pin);
    void begin();
    void update();

private:
    uint8_t pin;
    DHT dht;
};

#endif
