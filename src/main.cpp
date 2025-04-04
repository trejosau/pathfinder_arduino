#include <Arduino.h>
#include "button_manager.h"
#include "mq7_manager.h"
#include "mq4_manager.h"
#include "DHT11.h"
#include <nvs_flash.h>
#include "voltage_sensor.h"
#include "incline_sensor.h"

// ====================
// DEFINICIONES Y CONSTANTES
// ====================

// Variables del gps
#define RXD2 16
#define TXD2 17
#define GPS_BAUD 9600
// Intervalo de impresión para resumen de sensores
unsigned long lastPrintTime = 0;
const long printInterval = 7000; // 7 segundos

// ====================
// INSTANCIAS DE CLASES
// ====================
ButtonManager button(4);
DHT11 dht(27);
MQ7Manager mq7;
MQ4Manager mq4;
VoltageSensor voltageSensor;
HardwareSerial gpsSerial(2);
InclineSensor inclineSensor(15);


// Usamos el pin 25 como indicador LED para el sensor de inclinación (para no interferir con el GPS)
#define INDICATOR_LED_PIN 25

// ====================
// FUNCIONES AUXILIARES
// ====================
void initNVS() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    Serial.println("NVS initialized correctly");
}

// ====================
// SETUP
// ====================
void setup() {
    Serial.begin(115200);
    // Configuramos el pin indicador (LED) para el sensor de inclinación
    pinMode(INDICATOR_LED_PIN, OUTPUT);

    initNVS();
    delay(500);

    gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
    Serial.println("Serial 2 started at 9600 baud rate");


    Serial.println("Initializing DHT...");
    dht.begin();
    delay(100);

    Serial.println("Initializing Button Manager...");
    button.begin();
    delay(100);

    Serial.println("Initializing Voltage Sensor...");
    voltageSensor.begin();
    delay(100);

    Serial.println("Initializing MQ7...");
    mq7.begin();
    delay(100);

    Serial.println("Initializing MQ4...");
    mq4.begin();
    delay(100);

    Serial.println("Initializing Incline Sensor...");
    inclineSensor.begin();
    delay(100);

    Serial.println("\n--- Starting Pathfinder ---");
    Serial.println("Setup completed!");

}

// ====================
// LOOP
// ====================
void loop() {
    // Actualizar estado de sensores y botones
    button.update();

    // Leer y mostrar valores del DHT11
    if (dht.read()) {
        Serial.print("Humedad: ");
        Serial.print(dht.getHumidity());
        Serial.print(" % | Temperatura: ");
        Serial.print(dht.getTemperature());
        Serial.println(" °C");
    }

    // Actualizar y mostrar valores de MQ4 y MQ7
    mq4.update();
    mq7.update();

    Serial.print("MQ4 raw: ");
    Serial.println(mq4.getRawValue());
    Serial.print("MQ7 raw: ");
    Serial.println(mq7.getRawValue());
    Serial.print("Voltage: ");
    Serial.println(voltageSensor.getVoltage());
    Serial.print("Incline: ");
    Serial.println(inclineSensor.isInclined() ? "Inclined" : "Not inclined");

    // Control del LED indicador en INDICATOR_LED_PIN según el sensor de inclinación
    if (inclineSensor.isInclined()) {
        digitalWrite(INDICATOR_LED_PIN, LOW);
    } else {
        digitalWrite(INDICATOR_LED_PIN, HIGH);
    }

    // Imprimir un resumen de sensores cada 'printInterval'
    unsigned long currentTime = millis();
    if (currentTime - lastPrintTime >= printInterval) {
        lastPrintTime = currentTime;
        Serial.println("\n--- Resumen de Sensores ---");
        Serial.print("DHT: Temp=");
        Serial.print(dht.getTemperature());
        Serial.print(" °C, Humedad=");
        Serial.println(dht.getHumidity());
        Serial.print("MQ4: raw=");
        Serial.println(mq4.getRawValue());
        Serial.print("MQ7: raw=");
        Serial.println(mq7.getRawValue());
        Serial.print("Voltage: ");
        Serial.println(voltageSensor.getVoltage());
        Serial.print("Incline: ");
        Serial.println(inclineSensor.isInclined() ? "Inclined" : "Not inclined");
    }

    while (gpsSerial.available() > 0){
        // get the byte data from the GPS
        char gpsData = gpsSerial.read();
        Serial.print(gpsData);
    }
    Serial.println("-------------------------------");

    delay(1000);
}
