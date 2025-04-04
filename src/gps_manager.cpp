#include "gps_manager.h"

GPSManager::GPSManager(int rxPin, int txPin, int uartNum)
    : rxPin(rxPin), txPin(txPin), uartNum(uartNum), rawData("") {
    gpsSerial = new HardwareSerial(uartNum);
}

void GPSManager::begin(int baudRate) {
    // Inicializa el puerto serie del GPS:
    // Usamos rxPin para recibir datos (conectado al TX del GPS)
    gpsSerial->begin(baudRate, SERIAL_8N1, rxPin, txPin);
}

void GPSManager::update() {
    int bytesAvail = gpsSerial->available();
    Serial.print("Bytes disponibles en GPS: ");
    Serial.println(bytesAvail);  // Verifica si hay datos disponibles
    while (gpsSerial->available() > 0) {
        char c = gpsSerial->read();
        rawData += c;
    }
}


String GPSManager::getRawData() {
    // Devuelve el buffer de datos RAW y luego lo limpia
    String data = rawData;
    rawData = "";
    return data;
}
