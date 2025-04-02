#ifndef DHT11_H
#define DHT11_H

#include <Arduino.h>

class DHT11 {
private:
  uint8_t pin;
  uint8_t data[5];
  unsigned long lastReadTime;
  float temperature;
  float humidity;

  bool readData();
  uint8_t readByte();

public:
  DHT11(uint8_t pin);
  void begin();
  bool read();
  float getTemperature();
  float getHumidity();
};

#endif