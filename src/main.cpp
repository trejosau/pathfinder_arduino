#include <Arduino.h>
#include "button_manager.h"
#include "mq7_manager.h"
#include "mq4_manager.h"
#include "DHT11.h"
#include <nvs_flash.h>
#include "voltage_sensor.h"
#include "incline_sensor.h"
#include "TinyGPSPlus.h"
#include "preferences.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ====================
// DEFINICIONES Y CONSTANTES
// ====================

// Variables del GPS
#define RXD2 16
#define TXD2 17
#define GPS_BAUD 9600

// Intervalo de impresión para resumen de sensores
unsigned long lastPrintTime = 0;
const unsigned long PRINT_INTERVAL = 5000;  // 5 segundos

// Intervalo de publicación MQTT
unsigned long lastPublishTime = 0;
const unsigned long MQTT_PUBLISH_INTERVAL = 5000;

// Configuración WiFi y MQTT
const char* ssid = "pathfinder";
const char* password = "12345677";
const char* mqtt_server = "18.191.91.135";
const int mqtt_port = 1883;
const char* mqtt_user = "admin";
const char* mqtt_password = "2317Server_";

// ====================
// INSTANCIAS DE CLASES
// ====================
ButtonManager button(4);
DHT11 dht(27);
MQ7Manager mq7;
MQ4Manager mq4;
VoltageSensor voltageSensor;
HardwareSerial gpsSerial(2);
TinyGPSPlus gps;
InclineSensor inclineSensor(15);
Preferences preferences;

// Instancias para WiFi y MQTT
WiFiClient espClient;
PubSubClient mqttClient(espClient);

#define INDICATOR_BUZZE_PIN 25

// ID del dispositivo
String deviceId = "03cd2670-bd60-40d4-aecf-6db4ea6b62a7";

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
    Serial.println("NVS inicializado correctamente");
}

// Conectar a WiFi
void setupWiFi() {
    delay(10);
    Serial.println();
    Serial.print("Conectando a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi conectado");
    Serial.println("Dirección IP: ");
    Serial.println(WiFi.localIP());
}

// Reconexión MQTT
void reconnectMQTT() {
    while (!mqttClient.connected()) {
        Serial.print("Intentando conexión MQTT...");

        // Crear un ID de cliente aleatorio
        String clientId = "Pathfinder-";
        clientId += String(random(0xffff), HEX);

        // Intentar conectar
        if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
            Serial.println("conectado");
        } else {
            Serial.print("falló, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" intentando de nuevo en 5 segundos");
            delay(5000);
        }
    }
}

// Publicar datos MQTT
void publishSensorData() {
    // Preparar JSON para cada tipo de sensor
    StaticJsonDocument<200> gpsDoc;
    StaticJsonDocument<200> temperatureDoc;
    StaticJsonDocument<200> humidityDoc;
    StaticJsonDocument<200> mq4Doc;
    StaticJsonDocument<200> mq7Doc;
    StaticJsonDocument<200> voltageDoc;
    StaticJsonDocument<200> inclinationDoc;

    char buffer[256];
    String topic;

    // GPS
    if (gps.location.isValid()) {
        gpsDoc["lat"] = gps.location.lat();
        gpsDoc["lng"] = gps.location.lng();

        topic = "devices/" + deviceId + "/gps";
        serializeJson(gpsDoc, buffer);
        mqttClient.publish(topic.c_str(), buffer);
        Serial.println("Publicado: " + topic + " - " + String(buffer));
    }

    // Temperatura
    temperatureDoc["value"] = dht.getTemperature();
    temperatureDoc["unit"] = "C";

    topic = "devices/" + deviceId + "/temperature";
    serializeJson(temperatureDoc, buffer);
    mqttClient.publish(topic.c_str(), buffer);
    Serial.println("Publicado: " + topic + " - " + String(buffer));

    // Humedad
    humidityDoc["value"] = dht.getHumidity();
    humidityDoc["unit"] = "%";

    topic = "devices/" + deviceId + "/humidity";
    serializeJson(humidityDoc, buffer);
    mqttClient.publish(topic.c_str(), buffer);
    Serial.println("Publicado: " + topic + " - " + String(buffer));

    // Sensor MQ4 (metano)
    mq4Doc["value"] = mq4.getRawValue();
    mq4Doc["unit"] = "ppm";

    topic = "devices/" + deviceId + "/mq4";
    serializeJson(mq4Doc, buffer);
    mqttClient.publish(topic.c_str(), buffer);
    Serial.println("Publicado: " + topic + " - " + String(buffer));

    // Sensor MQ7 (monóxido de carbono)
    mq7Doc["value"] = mq7.getRawValue();
    mq7Doc["unit"] = "ppm";

    topic = "devices/" + deviceId + "/mq7";
    serializeJson(mq7Doc, buffer);
    mqttClient.publish(topic.c_str(), buffer);
    Serial.println("Publicado: " + topic + " - " + String(buffer));

    // Sensor de voltaje
    voltageDoc["value"] = voltageSensor.getVoltage();
    voltageDoc["unit"] = "V";

    topic = "devices/" + deviceId + "/voltage";
    serializeJson(voltageDoc, buffer);
    mqttClient.publish(topic.c_str(), buffer);
    Serial.println("Publicado: " + topic + " - " + String(buffer));

    // Sensor de inclinación
    inclinationDoc["value"] = inclineSensor.isInclined() ? 1 : 0;

    topic = "devices/" + deviceId + "/inclination";
    serializeJson(inclinationDoc, buffer);
    mqttClient.publish(topic.c_str(), buffer);
    Serial.println("Publicado: " + topic + " - " + String(buffer));
}

// ====================
// SETUP
// ====================
void setup() {
    Serial.begin(115200);
    preferences.begin("pathfinder", false);
    // Configuramos el pin indicador (buzzer) para el sensor de inclinación
    pinMode(INDICATOR_BUZZE_PIN, OUTPUT);

    initNVS();
    delay(500);

    gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
    Serial.println("GPS iniciado. Esperando datos de ubicación...");

    Serial.println("Inicializando DHT...");
    dht.begin();
    delay(100);

    Serial.println("Inicializando Button Manager...");
    button.begin();
    delay(100);

    Serial.println("Inicializando Voltage Sensor...");
    voltageSensor.begin();
    delay(100);

    Serial.println("Inicializando MQ7...");
    mq7.begin();
    delay(100);

    Serial.println("Inicializando MQ4...");
    mq4.begin();
    delay(100);

    Serial.println("Inicializando Incline Sensor...");
    inclineSensor.begin();
    delay(100);

    // Configurar WiFi y MQTT
    setupWiFi();
    mqttClient.setServer(mqtt_server, mqtt_port);

    Serial.println("\n--- Iniciando Pathfinder ---");

    // Usar el deviceId almacenado o el predeterminado
    String storedDeviceId = preferences.getString("deviceId", "");
    if (storedDeviceId != "") {
        deviceId = storedDeviceId;
    } else {
        preferences.putString("deviceId", deviceId);
    }

    Serial.println("Setup completado!");
    Serial.println("Device ID: " + deviceId);
}

// ====================
// LOOP
// ====================
void loop() {
    // Verificar conexión MQTT
    if (!mqttClient.connected()) {
        reconnectMQTT();
    }
    mqttClient.loop();

    // Leer datos GPS en segundo plano
    while (gpsSerial.available() > 0) {
        char c = gpsSerial.read();
        gps.encode(c);
    }

    // Actualizar estado de sensores y botones
    button.update();
    dht.read();
    mq4.update();
    mq7.update();

    // Control del buzzer según el sensor de inclinación
    digitalWrite(INDICATOR_BUZZE_PIN, inclineSensor.isInclined() ? LOW : HIGH);

    // Imprimir un resumen de sensores cada PRINT_INTERVAL
    unsigned long currentTime = millis();
    if (currentTime - lastPrintTime >= PRINT_INTERVAL) {
        lastPrintTime = currentTime;

        Serial.println("\n--- RESUMEN DE SENSORES ---");

        // Datos de temperatura y humedad
        Serial.print("TEMP: ");
        Serial.print(dht.getTemperature());
        Serial.print(" °C | HUM: ");
        Serial.print(dht.getHumidity());
        Serial.println(" %");

        // Datos de gases
        Serial.print("MQ4: ");
        Serial.print(mq4.getRawValue());
        Serial.print(" | MQ7: ");
        Serial.println(mq7.getRawValue());

        // Datos de voltaje e inclinación
        Serial.print("VOLT: ");
        Serial.print(voltageSensor.getVoltage());
        Serial.print(" | INCL: ");


        if (inclineSensor.isInclined()) {
            Serial.println("SI");
            digitalWrite(INDICATOR_BUZZE_PIN, LOW);
        } else {
            Serial.println("NO");
            digitalWrite(INDICATOR_BUZZE_PIN, HIGH);
        }

        // Datos GPS
        if (gps.location.isValid()) {
            Serial.print("GPS: LAT=");
            Serial.print(gps.location.lat(), 6);
            Serial.print(" LON=");
            Serial.println(gps.location.lng(), 6);
        } else {
            Serial.println("GPS: SIN SEÑAL");
        }

        Serial.println("-------------------------------");
    }

    // Publicar datos por MQTT cada MQTT_PUBLISH_INTERVAL
    if (currentTime - lastPublishTime >= MQTT_PUBLISH_INTERVAL) {
        lastPublishTime = currentTime;
        publishSensorData();
    }
}