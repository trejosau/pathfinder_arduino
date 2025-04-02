#ifndef CONFIG_H
#define CONFIG_H

// Configuración MQTT con EMQX
#define MQTT_SERVER "broker.emqx.io"  // Reemplaza con la IP/URL de tu servidor EMQX
#define MQTT_PORT 1883
#define MQTT_USER "admin"           // Si usas autenticación en EMQX
#define MQTT_PASSWORD "2317Server_"    // Si usas autenticación

// Configuración del Cliente MQTT
#define CLIENT_ID "ESP32Client"

#endif
