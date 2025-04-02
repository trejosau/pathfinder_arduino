#include "gps_manager.h"

GPSManager::GPSManager(HardwareSerial &serialPort, uint8_t rxPin, uint8_t txPin)
    : gpsSerial(serialPort), rxPin(rxPin), txPin(txPin) {}

void GPSManager::begin() {
    // Inicializa el puerto serial para el GPS a 9600 baudios
    gpsSerial.begin(115200, SERIAL_8N1, rxPin, txPin);
}

void GPSManager::update() {
    // Lee todos los caracteres disponibles del GPS
    while (gpsSerial.available() > 0) {
        char c = gpsSerial.read();
        gps.encode(c);
    }
}

bool GPSManager::locationUpdated() {
    return gps.location.isUpdated();
}

double GPSManager::getLatitude() {
    return gps.location.lat();
}

double GPSManager::getLongitude() {
    return gps.location.lng();
}
