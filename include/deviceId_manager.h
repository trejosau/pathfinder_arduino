#ifndef DEVICE_ID_MANAGER_H
#define DEVICE_ID_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>

class DeviceIdManager {
public:
    DeviceIdManager();
    
    // Inicializa el gestor de ID de dispositivo
    void begin();
    
    // Obtiene el ID del dispositivo, generando uno nuevo si no existe
    String getDeviceId();
    
    // Establece un ID de dispositivo personalizado
    bool setDeviceId(const String& id);
    
    // Verifica si un ID de dispositivo ya existe en la memoria
    bool hasDeviceId();

private:
    Preferences preferences;
    String deviceId;
    bool initialized;
    
    // Genera un nuevo UUID único
    String generateUUID();
};

#endif // DEVICE_ID_MANAGER_H