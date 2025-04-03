#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "button_manager.h"
#include "mq7_manager.h"
#include "mq4_manager.h"
#include "DHT11.h"
#include <nvs_flash.h>
#include "voltage_sensor.h"
#include "incline_sensor.h"
#include "motor_controller.h" // Incluimos el nuevo controlador de motores
#include <ArduinoJson.h>      // Para procesar JSON

// Fixed WiFi credentials
const char* WIFI_SSID = "PATHFINDERWIFI";
const char* WIFI_PASSWORD = "12345677";

// MQTT server settings
const char* MQTT_SERVER = "18.191.91.135"; // Replace with your MQTT broker address
const int MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "ESP32Client";

// Device ID for MQTT topics
const char* DEVICE_ID = "db2596e8-fe29-409e-8039-4c22e9a47407";

// Topic format: device/[device-id]/[sensor-type]
String topicBase = "device/" + String(DEVICE_ID);

// Instances of classes
WiFiClient espClient;
PubSubClient mqtt(espClient);
ButtonManager button(4);
DHT11 dht(27);
MQ7Manager mq7;
MQ4Manager mq4;
VoltageSensor voltageSensor;
InclineSensor inclineSensor(15);
MotorController motors; // Instancia del controlador de motores

// PWM properties
#define PWM_FREQUENCY 5000
#define PWM_RESOLUTION 8
#define PWM_CHANNEL_A 0
#define PWM_CHANNEL_B 1

unsigned long lastPublishTime = 0;
const long publishInterval = 7000; // Publish every 5 seconds

// Callback para controlar los motores mediante MQTT utilizando JSON
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    // Creamos un documento JSON estático con un tamaño adecuado
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload, length);

    String commandStr = "";

    if (!error) {
        // Extraemos el valor asociado a la clave "command"
        if (doc.containsKey("command")) {
            commandStr = doc["command"].as<String>();
        } else {
            Serial.println("JSON recibido pero no contiene la clave \"command\"");
            return;
        }
    } else {
        Serial.print("Error al parsear JSON: ");
        Serial.println(error.c_str());
        return;
    }

    String topicStr = String(topic);
    Serial.print("MQTT mensaje recibido [");
    Serial.print(topicStr);
    Serial.print("]: ");
    Serial.println(commandStr);

    // Procesar comandos para control de motores
    if (topicStr == topicBase + "/motor/control") {
        if (commandStr == "forward") {
            motors.moveForward(200); // Velocidad media
            Serial.println("Motores: Avanzando");
        }
        else if (commandStr == "backward") {
            motors.moveBackward(200);
            Serial.println("Motores: Retrocediendo");
        }
        else if (commandStr == "left") {
            motors.turnLeft(180);
            Serial.println("Motores: Girando izquierda");
        }
        else if (commandStr == "right") {
            motors.turnRight(180);
            Serial.println("Motores: Girando derecha");
        }
        else if (commandStr == "stop") {
            motors.stop();
            Serial.println("Motores: Detenidos");
        }
    }
    // También se puede añadir un tópico para control de velocidad (se espera un número en formato string dentro de "command")
    else if (topicStr == topicBase + "/motor/speed") {
        int speed = commandStr.toInt();
        if (speed >= 0 && speed <= 255) {
            motors.setSpeed(speed);
            Serial.print("Velocidad de motores ajustada a: ");
            Serial.println(speed);
        } else {
            Serial.println("Valor de velocidad fuera de rango (0-255)");
        }
    }
}

void initNVS() {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    Serial.println("NVS initialized correctly");
}

void setupWifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to WiFi: ");
    Serial.println(WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
    // Loop until we're reconnected to MQTT broker
    while (!mqtt.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqtt.connect(MQTT_CLIENT_ID)) {
            Serial.println("connected");
            // Suscribirse a los topics de control de motores
            mqtt.subscribe((topicBase + "/motor/control").c_str());
            mqtt.subscribe((topicBase + "/motor/speed").c_str());
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqtt.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void publishSensorData() {
    // Prepare topics for each sensor
    String dhtTopic = topicBase + "/dht";
    String mq4Topic = topicBase + "/mq4";
    String mq7Topic = topicBase + "/mq7";
    String voltageTopic = topicBase + "/voltage";
    String inclineTopic = topicBase + "/incline";

    // Publish DHT data
    if (dht.read()) {
        String dhtPayload = "{\"temperature\":" + String(dht.getTemperature()) +
                         ",\"humidity\":" + String(dht.getHumidity()) + "}";
        mqtt.publish(dhtTopic.c_str(), dhtPayload.c_str());

        Serial.print("Published to ");
        Serial.print(dhtTopic);
        Serial.print(": ");
        Serial.println(dhtPayload);
    }

    // Publish MQ4 (Methane) data
    String mq4Payload = "{\"raw\":" + String(mq4.getRawValue()) +
                      ",\"voltage\":" + String(mq4.getVoltage()) +
                      ",\"ppm\":" + String(mq4.getPPM()) + "}";
    mqtt.publish(mq4Topic.c_str(), mq4Payload.c_str());
    Serial.print("Published to ");
    Serial.print(mq4Topic);
    Serial.print(": ");
    Serial.println(mq4Payload);

    // Publish MQ7 (CO) data
    String mq7Payload = "{\"raw\":" + String(mq7.getRawValue()) +
                      ",\"voltage\":" + String(mq7.getVoltage()) +
                      ",\"ppm\":" + String(mq7.getPPM()) + "}";
    mqtt.publish(mq7Topic.c_str(), mq7Payload.c_str());
    Serial.print("Published to ");
    Serial.print(mq7Topic);
    Serial.print(": ");
    Serial.println(mq7Payload);

    // Publish Voltage data
    String voltagePayload = "{\"voltage\":" + String(voltageSensor.getVoltage()) +
                         ",\"percentage\":" + String(voltageSensor.getBatteryPercentage()) + "}";
    mqtt.publish(voltageTopic.c_str(), voltagePayload.c_str());
    Serial.print("Published to ");
    Serial.print(voltageTopic);
    Serial.print(": ");
    Serial.println(voltagePayload);

    // Publish Incline data
    String inclinePayload = "{\"inclined\":" + String(inclineSensor.isInclined() ? "true" : "false") + "}";
    mqtt.publish(inclineTopic.c_str(), inclinePayload.c_str());
    Serial.print("Published to ");
    Serial.print(inclineTopic);
    Serial.print(": ");
    Serial.println(inclinePayload);
}

void setup() {
    Serial.begin(115200);
    pinMode(13, OUTPUT);

    initNVS();
    delay(500);

    // Configurar canales PWM para los motores
    ledcSetup(PWM_CHANNEL_A, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_B, PWM_FREQUENCY, PWM_RESOLUTION);

    // Asignar los pines a los canales PWM
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

    // Setup WiFi connection with static credentials
    setupWifi();

    // Setup MQTT connection
    mqtt.setServer(MQTT_SERVER, MQTT_PORT);
    mqtt.setCallback(mqtt_callback); // Configurar callback para mensajes entrantes

    Serial.println("\n\n--- Starting Pathfinder ---");
    Serial.println("Device ID: " + String(DEVICE_ID));
    Serial.println("Setup completed!");
}

void loop() {
    // Check WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost. Reconnecting...");
        setupWifi();
    }

    // Check MQTT connection
    if (!mqtt.connected()) {
        reconnectMQTT();
    }
    mqtt.loop();



    // Read incline sensor
    if (inclineSensor.isInclined()) {
        digitalWrite(13, LOW);
    } else {
        digitalWrite(13, HIGH);
    }

    // Publish sensor data at regular intervals
    unsigned long currentTime = millis();
    if (currentTime - lastPublishTime >= publishInterval) {
        lastPublishTime = currentTime;
        publishSensorData();
    }

    delay(1000);
}
