#include <Arduino.h>
#include "wifi_manager.h"
#include "button_manager.h"
#include "mq7_manager.h"
#include "mq4_manager.h"
#include "DHT11.h"
#include <nvs_flash.h>
#include "voltage_sensor.h"
#include "incline_sensor.h"

// Instancias de las demás clases
WifiManager wifiManager;
ButtonManager button(4);
DHT11 dht(27);
MQ7Manager mq7;
MQ4Manager mq4;
VoltageSensor voltageSensor;
InclineSensor inclineSensor(15);

unsigned long lastPublishTime = 0;

void initNVS() {
    // Inicializar NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    Serial.println("NVS inicializado correctamente");
}

void setup() {
    Serial.begin(115200);


    initNVS();
    delay(500);

    Serial.println("Iniciando DHT...");
    dht.begin();
    delay(100);

    Serial.println("Iniciando Button Manager...");
    button.begin();
    delay(100);

    Serial.println("Iniciando Sensor de Voltaje...");
    voltageSensor.begin();
    delay(100);

    Serial.println("Iniciando MQ7...");
    mq7.begin();
    delay(100);

    Serial.println("Iniciando MQ4...");
    mq4.begin();
    delay(100);

    Serial.println("Iniciando WiFi Manager...");
    wifiManager.begin();
    delay(500);

    delay(1000);
    const String deviceId = wifiManager.getDeviceId();
    Serial.println("\n\n--- Iniciando pathfinder ---");
    Serial.println("Device ID: " + deviceId);

    Serial.println("Iniciando Sensor de Inclinación...");
    inclineSensor.begin();
    delay(100);


    Serial.println("Setup completado!");



}

void loop() {
    wifiManager.loop();
    button.update();
    mq7.update();
    mq4.update();
    voltageSensor.update();

    // Leer el sensor DHT11
    if (dht.read()) {
        Serial.print("Humedad: ");
        Serial.print(dht.getHumidity());
        Serial.print(" % | Temperatura: ");
        Serial.print(dht.getTemperature());
        Serial.println(" °C");
    }

    // Leer el sensor de inclinación
    if (inclineSensor.isInclined()) {
        Serial.println("Sensor de inclinación: ACTIVADO");
    } else {
        Serial.println("Sensor de inclinación: inactivo");
    }

    delay(1000);
}
