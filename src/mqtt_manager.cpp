#include "mqtt_manager.h"

MQTTManager::MQTTManager(const char* server, int port, const char* user, const char* password)
    : mqtt_server(server), mqtt_port(port), mqtt_user(user), mqtt_password(password), client(espClient) {
}

void MQTTManager::begin(const String& deviceId) {
    this->deviceId = deviceId;
    client.setServer(mqtt_server, mqtt_port);

    // Establecer el buffer de cliente para mensajes más grandes
    client.setBufferSize(512);

    Serial.println("MQTT Manager inicializado");
    Serial.print("Servidor MQTT: ");
    Serial.println(mqtt_server);
    Serial.print("Puerto MQTT: ");
    Serial.println(mqtt_port);
    Serial.print("ID de dispositivo: ");
    Serial.println(deviceId);
}

void MQTTManager::loop() {
    if (!client.connected()) {
        // Intentar reconectar cada 5 segundos
        unsigned long now = millis();
        if (now - lastReconnectAttempt > 5000) {
            lastReconnectAttempt = now;
            connect();
        }
    } else {
        client.loop();
    }
}

void MQTTManager::connect() {
    Serial.println("Intentando conectar al servidor MQTT...");

    // Crear un ID de cliente aleatorio
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    // Intentar conectar
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
        Serial.println("Conectado al servidor MQTT");

        // Publicar mensaje de conexión
        String connectTopic = "device/" + deviceId + "/status";
        DynamicJsonDocument doc(256);
        doc["status"] = "connected";
        doc["ip"] = WiFi.localIP().toString();
        doc["rssi"] = WiFi.RSSI();

        String payload;
        serializeJson(doc, payload);

        client.publish(connectTopic.c_str(), payload.c_str());
    } else {
        Serial.print("Error al conectar al MQTT, rc=");
        Serial.print(client.state());
        Serial.println(" Reintentando en 5 segundos");
    }
}

bool MQTTManager::isConnected() {
    return client.connected();
}

const char* MQTTManager::getTopic(const char* sensorType) {
    snprintf(topicBuffer, sizeof(topicBuffer), "device/%s/%s", deviceId.c_str(), sensorType);
    return topicBuffer;
}

bool MQTTManager::publish(const char* topic, const char* payload) {
    if (!client.connected()) {
        return false;
    }

    return client.publish(topic, payload);
}

void MQTTManager::publishDHT(float temperature, float humidity) {
    if (!client.connected()) {
        return;
    }

    const char* topic = getTopic("dht11");

    DynamicJsonDocument doc(256);
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["timestamp"] = millis();

    String payload;
    serializeJson(doc, payload);

    if (client.publish(topic, payload.c_str())) {
        Serial.println("Datos DHT11 publicados correctamente");
    } else {
        Serial.println("Error al publicar datos DHT11");
    }
}

void MQTTManager::publishMQ4(float rawValue, float voltage, float ppm) {
    if (!client.connected()) {
        return;
    }

    const char* topic = getTopic("mq4");

    DynamicJsonDocument doc(256);
    doc["raw_value"] = rawValue;
    doc["voltage"] = voltage;
    doc["ppm_ch4"] = ppm;
    doc["timestamp"] = millis();

    String payload;
    serializeJson(doc, payload);

    if (client.publish(topic, payload.c_str())) {
        Serial.println("Datos MQ4 publicados correctamente");
    } else {
        Serial.println("Error al publicar datos MQ4");
    }
}

void MQTTManager::publishMQ7(float rawValue, float voltage, float ppm) {
    if (!client.connected()) {
        return;
    }

    const char* topic = getTopic("mq7");

    DynamicJsonDocument doc(256);
    doc["raw_value"] = rawValue;
    doc["voltage"] = voltage;
    doc["ppm_co"] = ppm;
    doc["timestamp"] = millis();

    String payload;
    serializeJson(doc, payload);

    if (client.publish(topic, payload.c_str())) {
        Serial.println("Datos MQ7 publicados correctamente");
    } else {
        Serial.println("Error al publicar datos MQ7");
    }
}

void MQTTManager::publishVoltage(float voltage, float percentage) {
    if (!client.connected()) {
        return;
    }

    const char* topic = getTopic("battery");

    DynamicJsonDocument doc(256);
    doc["voltage"] = voltage;
    doc["percentage"] = percentage;
    doc["timestamp"] = millis();

    String payload;
    serializeJson(doc, payload);

    if (client.publish(topic, payload.c_str())) {
        Serial.println("Datos de batería publicados correctamente");
    } else {
        Serial.println("Error al publicar datos de batería");
    }
}

void MQTTManager::publishIncline(bool isInclined) {
    if (!client.connected()) {
        return;
    }

    const char* topic = getTopic("incline");

    DynamicJsonDocument doc(256);
    doc["inclined"] = isInclined;
    doc["timestamp"] = millis();

    String payload;
    serializeJson(doc, payload);

    if (client.publish(topic, payload.c_str())) {
        Serial.println("Datos de inclinación publicados correctamente");
    } else {
        Serial.println("Error al publicar datos de inclinación");
    }
}