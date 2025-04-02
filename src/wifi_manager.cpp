#include "wifi_manager.h"
#include <Preferences.h>
#include <WiFi.h>

// Instancia para guardar las credenciales en memoria no volátil
Preferences preferences;

WifiManager::WifiManager() : apMode(false), lastCheck(0) {
  // Constructor
}

void WifiManager::begin() {
  // Inicializar preferences
  preferences.begin("wifi-config", false);

  // Configurar el WiFi en modo STA (station)
  WiFi.mode(WIFI_STA);

  // Intentar conectarse con credenciales guardadas (si existen)
  String savedSSID = preferences.getString("ssid", "");
  String savedPassword = preferences.getString("password", "");

  if (savedSSID != "") {
    Serial.println("Intentando conectar con credenciales guardadas...");
    Serial.print("SSID: ");
    Serial.println(savedSSID);

    WiFi.begin(savedSSID.c_str(), savedPassword.c_str());

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
  }

  // Si no está conectado, configurar el AP y registrar eventos
  checkAndSetupAP();

  // Inicializar el servidor API
  webServer.begin();
}

bool WifiManager::isConnected() {
  return (WiFi.status() == WL_CONNECTED);
}

void WifiManager::checkAndSetupAP() {
  if (!isConnected()) {
    // No está conectado: configurar el modo AP
    const char* apSSID = "ESP32_Config";      // Nombre del AP
    const char* apPassword = "12345678";        // Contraseña (mínimo 8 caracteres)
    Serial.println("No está conectado a WiFi. Creando punto de acceso...");

    // Cambiar a modo AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSSID, apPassword);

    // Mostrar IP del AP
    IPAddress apIP = WiFi.softAPIP();
    Serial.print("AP creado con SSID: ");
    Serial.println(apSSID);
    Serial.print("Dirección IP del AP: ");
    Serial.println(apIP);

    apMode = true;

    // Registrar eventos para depurar conexiones de clientes
    // Registrar eventos para depurar conexiones de clientes
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
      if (event == ARDUINO_EVENT_WIFI_AP_STACONNECTED) {
        Serial.print("Cliente conectado, MAC: ");
        char mac[18];
        sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
                info.wifi_ap_staconnected.mac[0],
                info.wifi_ap_staconnected.mac[1],
                info.wifi_ap_staconnected.mac[2],
                info.wifi_ap_staconnected.mac[3],
                info.wifi_ap_staconnected.mac[4],
                info.wifi_ap_staconnected.mac[5]);
        Serial.println(mac);
      } else if (event == ARDUINO_EVENT_WIFI_AP_STADISCONNECTED) {
        Serial.print("Cliente desconectado, MAC: ");
        char mac[18];
        sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
                info.wifi_ap_stadisconnected.mac[0],
                info.wifi_ap_stadisconnected.mac[1],
                info.wifi_ap_stadisconnected.mac[2],
                info.wifi_ap_stadisconnected.mac[3],
                info.wifi_ap_stadisconnected.mac[4],
                info.wifi_ap_stadisconnected.mac[5]);
        Serial.println(mac);
      }
    });

  } else {
    Serial.println("Conectado a WiFi");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
    apMode = false;
  }
}

void WifiManager::tryConnect(const String &ssid, const String &password) {
  // Guardar las nuevas credenciales en NVS
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);

  Serial.println("Intentando conectar con nuevas credenciales...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  // Cambiar a modo estación e intentar la conexión
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (isConnected()) {
    Serial.println("Conectado exitosamente a la nueva red WiFi");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
    apMode = false;
  } else {
    Serial.println("Falló la conexión con las nuevas credenciales");
    // Si falla, se vuelve a configurar el AP
    checkAndSetupAP();
  }
}

void WifiManager::loop() {
  // Manejar las peticiones del servidor API
  webServer.handleClient();

  // Verificar si hay nuevas credenciales recibidas desde el endpoint /api/wifi
  String newSSID, newPassword;
  if (webServer.getNewCredentials(newSSID, newPassword)) {
    tryConnect(newSSID, newPassword);
    webServer.clearNewCredentials();
  }

  // Chequeo periódico de la conexión (cada 30 segundos)
  unsigned long currentTime = millis();
  if (currentTime - lastCheck > 30000) {
    lastCheck = currentTime;
    if (!isConnected() && !apMode) {
      Serial.println("Se perdió la conexión WiFi");
      checkAndSetupAP();
    }
  }
}
