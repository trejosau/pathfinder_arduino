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

unsigned long lastPublishTime = 0;
const long publishInterval = 5000; // Publish every 5 seconds

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

    // Setup WiFi connection with static credentials
    setupWifi();

    // Setup MQTT connection
    mqtt.setServer(MQTT_SERVER, MQTT_PORT);

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

    // Update sensor values
    button.update();
    mq7.update();
    mq4.update();
    voltageSensor.update();

    // Read DHT11 sensor
    if (dht.read()) {
        Serial.print("Humidity: ");
        Serial.print(dht.getHumidity());
        Serial.print(" % | Temperature: ");
        Serial.print(dht.getTemperature());
        Serial.println(" °C");
    }

    // Read incline sensor
    if (inclineSensor.isInclined()) {
        digitalWrite(13, LOW);
        Serial.println("Incline sensor: ACTIVATED");
    } else {
        Serial.println("Incline sensor: inactive");
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