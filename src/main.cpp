#include <Arduino.h>
#include "button_manager.h"
#include "mq7_manager.h"
#include "mq4_manager.h"
#include "DHT11.h"
#include <nvs_flash.h>
#include "voltage_sensor.h"
#include "incline_sensor.h"
#include "motor_controller.h"
#include "gps_manager.h"

// ====================
// DEFINICIONES Y CONSTANTES
// ====================

// PWM properties para motores
#define PWM_FREQUENCY 5000
#define PWM_RESOLUTION 8
#define PWM_CHANNEL_A 0
#define PWM_CHANNEL_B 1

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
InclineSensor inclineSensor(15);
MotorController motors;
// Para el GPS, usamos:
//   rxPin = 13 (por ejemplo, D13 en tu placa, que corresponde a GPIO13)
//   txPin = 17 (no se conecta si solo se leen datos)
GPSManager gps(13, 17);

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

    // Configurar canales PWM para motores
    ledcSetup(PWM_CHANNEL_A, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_B, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(MOTOR_ENA, PWM_CHANNEL_A);
    ledcAttachPin(MOTOR_ENB, PWM_CHANNEL_B);

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

    Serial.println("Initializing Motor Controller...");
    motors.begin();
    delay(100);

    Serial.println("\n--- Starting Pathfinder ---");
    Serial.println("Setup completed!");

    Serial.println("\n--- Iniciando GPS (Datos RAW) ---");
    gps.begin(9600);  // O prueba 4800 o 38400
    delay(100);
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

    // Actualizar y mostrar datos RAW del GPS
    gps.update();
    String gpsData = gps.getRawData();
    if (gpsData.length() > 0) {
        Serial.println("\n--- GPS RAW Data ---");
        Serial.print(gpsData);
    } else {
        Serial.println("Esperando datos RAW del GPS...");
    }

    delay(1000);
}
