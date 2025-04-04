#ifndef GPS_MANAGER_H
#define GPS_MANAGER_H

#include <Arduino.h>
#include <HardwareSerial.h>

class GPSManager {
public:
    // Constructor: rxPin y txPin se indican según el mapeo de tu placa (ej. D4 = GPIO16, D5 = GPIO17)
    GPSManager(int rxPin, int txPin, int uartNum = 2);
    void begin(int baudRate = 9600);
    void update();
    // Devuelve los datos RAW acumulados y limpia el buffer
    String getRawData();

private:
    HardwareSerial* gpsSerial;
    int uartNum;
    int rxPin;
    int txPin;
    String rawData;
};

#endif
