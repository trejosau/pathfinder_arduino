#include "deviceId_manager.h"

DeviceIdManager::DeviceIdManager() : initialized(false) {
    // Constructor
}

void DeviceIdManager::begin() {
    if (!initialized) {
        // Inicializar el espacio de nombres para almacenar el ID del dispositivo
        preferences.begin("device-config", false);
        initialized = true;
        
        // Verificar si ya existe un ID
        deviceId = preferences.getString("deviceId", "");
        
        // Si no existe, generar un nuevo UUID
        if (deviceId == "") {
            deviceId = generateUUID();
            preferences.putString("deviceId", deviceId);
            Serial.println("Nuevo dispositivo registrado con ID: " + deviceId);
        } else {
            Serial.println("Dispositivo ya registrado con ID: " + deviceId);
        }
    }
}

String DeviceIdManager::getDeviceId() {
    if (!initialized) {
        begin();
    }
    return deviceId;
}

bool DeviceIdManager::setDeviceId(const String& id) {
    if (!initialized) {
        begin();
    }
    
    // Validar que el ID no esté vacío
    if (id.length() == 0) {
        return false;
    }
    
    // Almacenar el nuevo ID
    deviceId = id;
    preferences.putString("deviceId", deviceId);
    Serial.println("ID de dispositivo actualizado: " + deviceId);
    return true;
}

bool DeviceIdManager::hasDeviceId() {
    if (!initialized) {
        begin();
    }
    return deviceId.length() > 0;
}

String DeviceIdManager::generateUUID() {
    uint32_t chipId = ESP.getEfuseMac();
    String uuid = "";
    
    // Crear un UUID basado en la dirección MAC y timestamp
    uint32_t timestamp = millis();
    char buffer[37];
    sprintf(buffer, "%08X-%04X-%04X-%04X-%08X%04X",
            chipId,
            random(0xFFFF), // parte aleatoria
            ((random(0x0FFF) | 0x4000)), // Versión 4
            (random(0x3FFF) | 0x8000), // Variante
            timestamp,
            random(0xFFFF));
    
    uuid = String(buffer);
    return uuid;
}