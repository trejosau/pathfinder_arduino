#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

class WiFiManager {
private:
    const char* _ssid;
    const char* _password;
    unsigned long _connectionTimeout;
    unsigned long _lastReconnectAttempt;
    unsigned long _reconnectInterval;
    bool _isConnected;

public:
    // Constructor with default values
    WiFiManager(const char* ssid = "", const char* password = "",
                unsigned long connectionTimeout = 20000, unsigned long reconnectInterval = 10000);

    void begin();
    void update();
    bool isConnected();
    void setCredentials(const char* ssid, const char* password);
    String getLocalIP();
    int getSignalStrength();
    void setReconnectInterval(unsigned long interval);
};

#endif // WIFI_MANAGER_H