#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WebServer.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <WiFi.h>

class WebServerManager {
public:
    WebServerManager();
    void begin();
    void handleClient();
    bool getNewCredentials(String &ssid, String &password);
    void clearNewCredentials();
    void setDeviceID(const String &deviceID);

private:
    WebServer server;
    Preferences preferences;
    bool credentialsReceived;
    String deviceID;

    void enableCORS();
    void handleGetStatus();
    void handleSetCredentials();
    void handleNotFound();
    void handleGetDeviceID();
};

#endif