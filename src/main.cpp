#include <Arduino.h>
#include "wifi_manager.h"
#include "button_manager.h"
#include "mq7_manager.h"
#include "mq4_manager.h"
#include "gps_manager.h"
#include "DHT11.h"
#include <nvs_flash.h>

// Instancia de HardwareSerial para el GPS (usando UART2)
HardwareSerial gpsSerial(2);

// Instancias de las demás clases (ajusta según tu configuración)
WifiManager wifiManager;
ButtonManager button(4);
DHT11 dht(27);
MQ7Manager mq7;
MQ4Manager mq4;
GPSManager gpsManager(gpsSerial, 3, 1);

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
    delay(1000); // Dar tiempo al puerto serie para inicializarse
    Serial.println("\n\n--- Iniciando aplicación ---");

    initNVS();
    delay(500);


    Serial.println("Iniciando DHT...");
    dht.begin();
    delay(100);

    Serial.println("Iniciando Button Manager...");
    button.begin();
    delay(100);

    Serial.println("Iniciando MQ7...");
    mq7.begin();
    delay(100);

    Serial.println("Iniciando MQ4...");
    mq4.begin();
    delay(100);

   // Serial.println("Iniciando GPS...");//gpsManager.begin();
    delay(100);

    // WiFi Manager al final para dar prioridad a los demás componentes
    Serial.println("Iniciando WiFi Manager...");
    wifiManager.begin();
    delay(500);

    Serial.println("Setup completado!");
}
void loop() {
    wifiManager.loop();
    button.update();
    mq7.update();
    mq4.update();

    // Leer el sensor DHT11
    delay(1000);
    if (dht.read()) {
        Serial.print("Humedad: ");
        Serial.print(dht.getHumidity());
        Serial.print(" % | Temperatura: ");
        Serial.print(dht.getTemperature());
        Serial.println(" °C");
    } else {
        Serial.println("Error al leer el sensor DHT11");
    }

    gpsManager.update();

    if (gpsManager.locationUpdated()) {
        Serial.print("Latitud: ");
        Serial.println(gpsManager.getLatitude());
        Serial.print("Longitud: ");
        Serial.println(gpsManager.getLongitude());
    }


    delay(1000); // Intervalo de actualización
}
