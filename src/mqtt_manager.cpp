#include "mqtt_manager.h"

MQTTManager::MQTTManager(DeviceIdManager& deviceIdManager) 
    : deviceIdManager(deviceIdManager), client(espClient), lastReconnectAttempt(0) {
}

void MQTTManager::begin(const char* server, int port) {
    deviceId = deviceIdManager.getDeviceId();
    client.setServer(server, port);
    
    Serial.print("MQTT Manager inicializado para el dispositivo: ");
    Serial.println(deviceId);
}

void MQTTManager::loop() {
    // Solo intenta conectar si hay WiFi
    if (WiFi.status() == WL_CONNECTED) {
        if (!client.connected()) {
            unsigned long now = millis();
            if (now - lastReconnectAttempt > RECONNECT_INTERVAL) {
                lastReconnectAttempt = now;
                // Intento de reconexión
                if (connectToMqtt()) {
                    lastReconnectAttempt = 0;
                }
            }
        } else {
            // Cliente conectado, mantener la conexión
            client.loop();
        }
    }
}

bool MQTTManager::isConnected() {
    return client.connected();
}

bool MQTTManager::connectToMqtt() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Sin conexión WiFi. No se puede conectar a MQTT.");
        return false;
    }

    Serial.print("Conectando a MQTT...");
    String clientId = "ESP32Client-";
    clientId += deviceId;
    
    if (client.connect(clientId.c_str())) {
        Serial.println("conectado");
        return true;
    } else {
        Serial.print("falló, rc=");
        Serial.print(client.state());
        Serial.println(" intentando de nuevo en 5 segundos");
        return false;
    }
}

void MQTTManager::createTopicForSensor(const char* sensorType, char* buffer, int bufferSize) {
    snprintf(buffer, bufferSize, "device/%s/%s", deviceId.c_str(), sensorType);
}

bool MQTTManager::publish(const char* sensorType, float value) {
    if (!isConnected() || WiFi.status() != WL_CONNECTED) {
        return false;
    }

    char topic[128];
    createTopicForSensor(sensorType, topic, sizeof(topic));

    char payload[10];
    dtostrf(value, 2, 2, payload);

    Serial.print("Publicando en tópico: ");
    Serial.print(topic);
    Serial.print(" | Valor: ");
    Serial.println(payload);

    return client.publish(topic, payload);
}

bool MQTTManager::publish(const char* sensorType, JsonDocument& doc) {
    if (!isConnected() || WiFi.status() != WL_CONNECTED) {
        return false;
    }

    char topic[128];
    createTopicForSensor(sensorType, topic, sizeof(topic));
    
    String payload;
    serializeJson(doc, payload);
    
    Serial.print("Publicando en tópico: ");
    Serial.print(topic);
    Serial.print(" | Payload: ");
    Serial.println(payload);
    
    return client.publish(topic, payload.c_str());
}

bool MQTTManager::publishTemperature(float temperature) {
    return publish("temperature", temperature);
}

bool MQTTManager::publishHumidity(float humidity) {
    return publish("humidity", humidity);
}

bool MQTTManager::publishCO(float ppmCO) {
    return publish("co", ppmCO);
}

bool MQTTManager::publishCH4(float ppmCH4) {
    return publish("ch4", ppmCH4);
}

bool MQTTManager::publishLocation(double latitude, double longitude) {
    StaticJsonDocument<200> doc;
    doc["latitude"] = latitude;
    doc["longitude"] = longitude;
    return publish("location", doc);
}