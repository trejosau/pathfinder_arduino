#include <Arduino.h>
#include "wifi_manager.h"
#include "button_manager.h"
#include "mq7_manager.h"
#include "mq4_manager.h"
#include "gps_manager.h"
#include "dht_manager.h"

// Instancia de HardwareSerial para el GPS (usando UART2)
HardwareSerial gpsSerial(2);

// Instancias de las demás clases (ajusta según tu configuración)
WifiManager wifiManager;
ButtonManager button(4);
DHTManager dhtManager(13);
MQ7Manager mq7;
MQ4Manager mq4;
GPSManager gpsManager(gpsSerial, 3, 1);

void setup() {
    Serial.begin(115200);
    Serial.println("Iniciando...");

    dhtManager.begin();
   // wifiManager.begin();
   // button.begin();
    mq7.begin();
    mq4.begin();
    gpsManager.begin();
}

void loop() {
    wifiManager.loop();
    button.update();
    mq7.update();
    mq4.update();
    dhtManager.update();
    gpsManager.update();

    if (gpsManager.locationUpdated()) {
        Serial.print("Latitud: ");
        Serial.println(gpsManager.getLatitude());
        Serial.print("Longitud: ");
        Serial.println(gpsManager.getLongitude());
    }

    delay(1000); // Intervalo de actualización
}
