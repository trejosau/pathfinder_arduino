#include "wifi_manager.h"

WiFiManager::WiFiManager(const char* ssid, const char* password,
                         unsigned long connectionTimeout, unsigned long reconnectInterval) {
    _ssid = ssid;
    _password = password;
    _connectionTimeout = connectionTimeout;
    _reconnectInterval = reconnectInterval;
    _lastReconnectAttempt = 0;
    _isConnected = false;
}

void WiFiManager::begin() {
    Serial.println("Iniciando conexión WiFi...");

    // Set WiFi mode
    WiFi.mode(WIFI_STA);

    // Start connection attempt
    WiFi.begin(_ssid, _password);

    // Wait for connection with timeout
    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < _connectionTimeout) {
        Serial.print(".");
        delay(500);
    }

    if (WiFi.status() == WL_CONNECTED) {
        _isConnected = true;
        Serial.println();
        Serial.println("WiFi conectado exitosamente!");
        Serial.print("Dirección IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("Intensidad de señal (RSSI): ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
    } else {
        _isConnected = false;
        Serial.println();
        Serial.println("Fallo en la conexión WiFi. Intentando de nuevo más tarde...");
        // Disconnect to clean up but don't stop trying
        WiFi.disconnect();
    }

    _lastReconnectAttempt = millis();
}

void WiFiManager::update() {
    // Check current connection status
    bool currentlyConnected = (WiFi.status() == WL_CONNECTED);

    // Update state
    if (_isConnected != currentlyConnected) {
        _isConnected = currentlyConnected;

        if (_isConnected) {
            Serial.println("WiFi reconectado exitosamente!");
            Serial.print("Dirección IP: ");
            Serial.println(WiFi.localIP());
            Serial.print("Intensidad de señal: ");
            Serial.print(WiFi.RSSI());
            Serial.println(" dBm");
        } else {
            Serial.println("Conexión WiFi perdida. Intentando reconexión automáticamente...");
        }
    }

    // Attempt reconnection if disconnected and enough time has passed
    if (!_isConnected && millis() - _lastReconnectAttempt > _reconnectInterval) {
        Serial.print("Intentando reconexión WiFi a '");
        Serial.print(_ssid);
        Serial.println("'...");

        // Sometimes it helps to disconnect explicitly before reconnecting
        WiFi.disconnect();
        delay(100);

        // Begin connection
        WiFi.begin(_ssid, _password);
        _lastReconnectAttempt = millis();
    }
}

bool WiFiManager::isConnected() {
    return _isConnected;
}

void WiFiManager::setCredentials(const char* ssid, const char* password) {
    _ssid = ssid;
    _password = password;
}

String WiFiManager::getLocalIP() {
    if (_isConnected) {
        return WiFi.localIP().toString();
    } else {
        return "No conectado";
    }
}

int WiFiManager::getSignalStrength() {
    if (_isConnected) {
        return WiFi.RSSI();
    } else {
        return 0;
    }
}

void WiFiManager::setReconnectInterval(unsigned long interval) {
    _reconnectInterval = interval;
}