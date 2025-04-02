#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>  // Biblioteca para MQTT
#include "wifi_manager.h"
#include "button_manager.h"
#include "mq7_manager.h"
#include "mq4_manager.h"
#include "DHT11.h"
#include <nvs_flash.h>
#include "config.h"  // Asegúrate de incluir tu archivo de configuraciones
#include <Preferences.h>  // Incluir librería Preferences para guardar el DEVICE_ID

// Instancias de las demás clases (ajusta según tu configuración)
WifiManager wifiManager;
ButtonManager button(4);
DHT11 dht(27);
MQ7Manager mq7;
MQ4Manager mq4;

// Instancia del cliente MQTT
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Instancia de Preferences
Preferences preferences;

// Función para obtener el DEVICE_ID desde la memoria flash
String getDeviceId() {
    preferences.begin("config", false);  // Abrir el almacenamiento "config"
    String deviceId = preferences.getString("DEVICE_ID", "default_id");  // Obtener el ID, si no existe devuelve "default_id"
    preferences.end();  // Cerrar el acceso a las preferencias
    return deviceId;
}

// Función para establecer el DEVICE_ID en la memoria flash (solo la primera vez)
void setDeviceId(const String& deviceId) {
    preferences.begin("config", false);  // Abrir el almacenamiento "config"
    preferences.putString("DEVICE_ID", deviceId);  // Guardar el DEVICE_ID
    preferences.end();  // Cerrar las preferencias
}

// Función para inicializar la memoria NVS
void initNVS() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    Serial.println("NVS inicializado correctamente");
}

// Función para conectar al WiFi
void setupWiFi() {
    Serial.println("Conectando a WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nWiFi conectado");
}

// Función para configurar MQTT
void setupMQTT() {
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(callback);
}

// Función para reconectar a MQTT
void reconnectMQTT() {
    while (!mqttClient.connected()) {
        Serial.print("Conectando a EMQX...");
        if (mqttClient.connect(CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
            Serial.println("Conectado");
            mqttClient.subscribe("device/+/command");  // Suscribir al tema adecuado
        } else {
            Serial.print("Error, rc=");
            Serial.println(mqttClient.state());
            delay(2000);
        }
    }
}

// Función callback para los mensajes MQTT
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Mensaje en tópico: ");
    Serial.println(topic);
    Serial.print("Mensaje: ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void publishData(const String& topic, const String& payload) {
    if (wifiManager.isConnected()) {  // Verifica si está conectado a WiFi antes de publicar
        if (!mqttClient.connected()) {
            reconnectMQTT();
        }
        mqttClient.publish(topic.c_str(), payload.c_str());
    } else {
        Serial.println("No hay conexión WiFi, no se puede publicar los datos.");
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);  // Dar tiempo al puerto serie para inicializarse
    Serial.println("\n\n--- Iniciando aplicación ---");

    initNVS();
    delay(500);

    // Inicialización de los módulos
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

    // Inicialización WiFi y MQTT
    Serial.println("Iniciando WiFi Manager...");
    wifiManager.begin();
    delay(500);

    setupWiFi();    // Conectar al WiFi
    setupMQTT();    // Configurar MQTT

    // Recuperar el DEVICE_ID desde la memoria flash
    String deviceId = getDeviceId();
    if (deviceId == "default_id") {
        // Si no existe un DEVICE_ID, establecer uno
        String newDeviceId = "ESP32_Device_001";  // Cambia este ID si deseas algo específico
        setDeviceId(newDeviceId);
        deviceId = newDeviceId;  // Asignar el nuevo DEVICE_ID
    }
    Serial.print("DEVICE_ID: ");
    Serial.println(deviceId);  // Mostrar el DEVICE_ID en el monitor serie

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

        // Obtener el DEVICE_ID y publicar datos en el broker MQTT
        String deviceId = getDeviceId();
        String topic = "device/" + deviceId + "/DHT11";
        String payload = "{\"temperature\": " + String(dht.getTemperature()) + ", \"humidity\": " + String(dht.getHumidity()) + "}";
        publishData(topic, payload);
    } else {
        Serial.println("Error al leer el sensor DHT11");
    }

    mqttClient.loop();  // Mantener la conexión MQTT
    delay(1000);  // Intervalo de actualización
}