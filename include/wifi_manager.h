#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "web_server.h"

class WifiManager {
public:
    WifiManager();
    void begin();
    void loop();
    bool isConnected();
    void tryConnect(const String &ssid, const String &password);
private:
    void checkAndSetupAP();
    Preferences preferences;
    bool apMode;
    unsigned long lastCheck;
    WebServerManager webServer;
};

#endif
