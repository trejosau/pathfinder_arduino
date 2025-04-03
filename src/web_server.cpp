#include "web_server.h"

WebServerManager::WebServerManager() : server(8090), credentialsReceived(false) {
    Serial.println("WebServerManager: Constructor inicializado");
}

String WebServerManager::getDeviceId() {
   String deviceId = preferences.getString("deviceId");
    return deviceId;
}

void WebServerManager::begin() {
    // Inicializar las preferencias aquí en lugar de en el constructor
    if (!preferences.begin("wificonfig", false)) {
        Serial.println("ERROR: No se pudo inicializar preferences en WebServerManager");
    } else {
        Serial.println("Preferences iniciado correctamente en WebServerManager");
    }

    Serial.println("Configurando rutas API...");

    // Configurar rutas API
    server.on("/api/status", HTTP_GET, [this]() {
        Serial.println("Endpoint /api/status accedido");
        this->handleGetStatus();
    });

    Serial.println("Ruta /api/status configurada");

    server.on("/api/wifi", HTTP_POST, [this]() {
        Serial.println("Endpoint /api/wifi accedido");
        this->handleSetCredentials();
    });

    Serial.println("Ruta /api/wifi configurada");

    server.onNotFound([this]() {
        Serial.println("Endpoint no encontrado accedido");
        this->handleNotFound();
    });

    Serial.println("Ruta 404 configurada");

    // Iniciar servidor
    Serial.println("Iniciando servidor...");
    server.begin();
    Serial.println("Servidor API iniciado en el puerto 8090");
}

void WebServerManager::handleClient() {
    server.handleClient();
}

bool WebServerManager::getNewCredentials(String &ssid, String &password) {
    if (credentialsReceived) {
        // Leer credenciales desde la memoria flash
        ssid = preferences.getString("SSIDWIFI", "");
        password = preferences.getString("PASSWORDWIFI", "");
        Serial.println("Nuevas credenciales obtenidas desde flash: SSID=" + ssid);
        return true;
    }
    return false;
}

// Función que faltaba y causaba el error de enlace
void WebServerManager::clearNewCredentials() {
    credentialsReceived = false;
    Serial.println("Credenciales marcadas como procesadas");
}

void WebServerManager::enableCORS() {
    // Configurar cabeceras para permitir peticiones desde la app
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void WebServerManager::handleGetStatus() {
    enableCORS();

    // Crear JSON con el estado
    DynamicJsonDocument doc(512);

    if (WiFi.status() == WL_CONNECTED) {
        doc["connected"] = true;
        doc["ssid"] = WiFi.SSID();
        doc["ip"] = WiFi.localIP().toString();
        doc["rssi"] = WiFi.RSSI();
        Serial.println("WiFi conectado: " + WiFi.SSID());
    } else {
        doc["connected"] = false;
        doc["mode"] = "AP";
        doc["ap_ssid"] = WiFi.softAPSSID();
        doc["ap_ip"] = WiFi.softAPIP().toString();
        Serial.println("WiFi en modo AP: " + WiFi.softAPSSID());
    }

    String response;
    serializeJson(doc, response);

    Serial.println("Enviando respuesta de estado: " + response);
    server.send(200, "application/json", response);
}

void WebServerManager::handleSetCredentials() {
    enableCORS();
    Serial.println("Procesando solicitud de cambio de credenciales...");

    // Imprimir todos los headers para depuración
    Serial.println("---- Headers recibidos ----");
    for (int i = 0; i < server.headers(); i++) {
        Serial.print(server.headerName(i));
        Serial.print(": ");
        Serial.println(server.header(i));
    }
    Serial.println("-------------------------");

    // Verificar si hay datos en el cuerpo
    if (server.hasArg("plain")) {
        String body = server.arg("plain");
        Serial.print("Cuerpo de la petición: ");
        Serial.println(body);

        // Más flexible: verificar si contiene application/json o si hay un cuerpo JSON válido
        bool hasJsonContentType = server.hasHeader("Content-Type") &&
                                server.header("Content-Type").indexOf("application/json") >= 0;
        bool hasJsonBody = body.length() > 0 && body.startsWith("{");

        if (hasJsonContentType || hasJsonBody) {
            // Analizar JSON
            DynamicJsonDocument doc(512);
            DeserializationError error = deserializeJson(doc, body);

            if (!error) {
                Serial.println("JSON recibido y parseado correctamente");
                // Verificar si el JSON contiene los campos requeridos
                if (doc.containsKey("ssid") && doc.containsKey("password")) {
                    String ssid = doc["ssid"].as<String>();
                    String password = doc["password"].as<String>();

                    // Intentar conectarse al WiFi para validar las credenciales
                    Serial.println("Intentando conectar al WiFi con SSID: " + ssid);

                    // Guardar modo AP actual
                    IPAddress apIP = WiFi.softAPIP();
                    String apSSID = WiFi.softAPSSID();

                    // Intentar conexión al WiFi
                    WiFi.begin(ssid.c_str(), password.c_str());

                    // Esperar hasta 10 segundos para la conexión
                    int timeout = 10; // 10 segundos
                    bool connected = false;

                    while (timeout > 0 && WiFi.status() != WL_CONNECTED) {
                        delay(1000);
                        Serial.print(".");
                        timeout--;
                    }

                    if (WiFi.status() == WL_CONNECTED) {
                        // Conexión exitosa, las credenciales son correctas
                        Serial.println("\nConexión exitosa al WiFi: " + ssid);
                        Serial.println("IP asignada: " + WiFi.localIP().toString());

                        // Guardar las credenciales en la memoria flash
                        preferences.putString("SSIDWIFI", ssid);
                        preferences.putString("PASSWORDWIFI", password);
                        credentialsReceived = true;

                        String deviceId = preferences.getString("deviceId");

                        // Preparar respuesta de éxito
                        DynamicJsonDocument response(256);
                        response["success"] = true;
                        response["message"] = "Credenciales verificadas y guardadas correctamente";
                        response["deviceId"] = deviceId;
                        response["ip"] = WiFi.localIP().toString();

                        String jsonResponse;
                        serializeJson(response, jsonResponse);

                        Serial.println("Enviando respuesta exitosa: " + jsonResponse);
                        server.send(200, "application/json", jsonResponse);

                        // No regresamos al modo AP inmediatamente para mantener la conexión
                    } else {
                        // Las credenciales son incorrectas
                        Serial.println("\nError: No se pudo conectar al WiFi. Credenciales incorrectas o red no disponible.");

                        // Volver al modo AP
                        WiFi.disconnect();
                        delay(500);
                        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
                        WiFi.softAP(apSSID.c_str());

                        // Respuesta de error
                        DynamicJsonDocument response(256);
                        response["success"] = false;
                        response["message"] = "No se pudo conectar al WiFi. Credenciales incorrectas o red no disponible.";

                        String jsonResponse;
                        serializeJson(response, jsonResponse);

                        Serial.println("Enviando respuesta de error: " + jsonResponse);
                        server.send(400, "application/json", jsonResponse);
                    }
                } else {
                    Serial.println("Error: Faltan campos requeridos en el JSON");
                    server.send(400, "application/json", "{\"success\":false,\"message\":\"Faltan campos requeridos (ssid y/o password)\"}");
                }
            } else {
                Serial.print("Error al parsear JSON: ");
                Serial.println(error.c_str());
                server.send(400, "application/json", "{\"success\":false,\"message\":\"Error al procesar JSON\"}");
            }
        } else {
            Serial.println("Error: Content-Type incorrecto o cuerpo no reconocido como JSON");
            server.send(400, "application/json", "{\"success\":false,\"message\":\"Se requiere Content-Type: application/json o cuerpo JSON válido\"}");
        }
    } else {
        Serial.println("Error: No hay datos en el cuerpo de la petición");
        server.send(400, "application/json", "{\"success\":false,\"message\":\"No hay datos en la petición\"}");
    }
}

void WebServerManager::handleNotFound() {
    enableCORS();

    // Para peticiones OPTIONS (preflight CORS)
    if (server.method() == HTTP_OPTIONS) {
        Serial.println("Petición OPTIONS recibida");
        server.send(204);
        return;
    }

    Serial.println("404 - Endpoint no encontrado");
    server.send(404, "application/json", "{\"success\":false,\"message\":\"Endpoint no encontrado\"}");
}
