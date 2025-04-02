#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "deviceId_manager.h"

class MQTTManager {
public:
    MQTTManager(DeviceIdManager& deviceIdManager);
    void begin(const char* server, int port);
    void loop();
    bool isConnected();
    bool publish(const char* sensorType, float value);
    bool publish(const char* sensorType, JsonDocument& doc);
    bool publishTemperature(float temperature);
    bool publishHumidity(float humidity);
    bool publishCO(float ppmCO);
    bool publishCH4(float ppmCH4);
    bool publishLocation(double latitude, double longitude);

private:
    bool connectToMqtt();
    void createTopicForSensor(const char* sensorType, char* buffer, int bufferSize);

    DeviceIdManager& deviceIdManager;
    WiFiClient espClient;
    PubSubClient client;
    String deviceId;
    unsigned long lastReconnectAttempt;
    const int RECONNECT_INTERVAL = 5000; // 5 segundos entre intentos de reconexión
};

#endif