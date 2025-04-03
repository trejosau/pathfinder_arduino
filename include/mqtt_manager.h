#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

class MQTTManager {
public:
    MQTTManager(const char* mqtt_server, int mqtt_port, const char* mqtt_user, const char* mqtt_password);
    void begin(const String& deviceId);
    void loop();
    bool publish(const char* topic, const char* payload);
    bool isConnected();

    // Métodos para publicar datos de sensores específicos
    void publishDHT(float temperature, float humidity);
    void publishMQ4(float rawValue, float voltage, float ppm);
    void publishMQ7(float rawValue, float voltage, float ppm);
    void publishVoltage(float voltage, float percentage);
    void publishIncline(bool isInclined);

private:
    void connect();

    // Configuración MQTT
    WiFiClient espClient;
    PubSubClient client;
    const char* mqtt_server;
    int mqtt_port;
    const char* mqtt_user;
    const char* mqtt_password;
    String deviceId;
    unsigned long lastReconnectAttempt = 0;

    // Buffer para tópicos MQTT
    char topicBuffer[128];

    // Función para generar el tópico completo
    const char* getTopic(const char* sensorType);
};

#endif