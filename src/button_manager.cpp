// ButtonManager.cpp
#include "button_manager.h"

ButtonManager::ButtonManager(uint8_t pin) : pin(pin), lastState(HIGH), currentState(HIGH), pressStartTime(0), messageShown(false) {}

void ButtonManager::begin() {
    pinMode(pin, INPUT_PULLUP);
    // No necesitamos iniciar preferences aquí, solo cuando las usemos
}

void ButtonManager::update() {
    currentState = digitalRead(pin);

    // Verificar pulsación del botón
    if (lastState == HIGH && currentState == LOW) {
        pressStartTime = millis();
        messageShown = false;
    }

    // Detección de pulsación larga
    if (currentState == LOW && (millis() - pressStartTime >= 3000) && !messageShown) {
        Serial.println("Botón presionado por 3 segundos. Suelta el botón para borrar las credenciales WiFi...");
        messageShown = true;
    }

    // Cuando se suelta el botón después de una pulsación larga
    if (lastState == LOW && currentState == HIGH && messageShown) {
        Serial.println("Borrando credenciales WiFi...");

        // Iniciar preferences con el mismo namespace usado para guardar las credenciales
        preferences.begin("wificonfig", false);

        // Borrar específicamente las credenciales WiFi
        preferences.remove("SSIDWIFI");
        preferences.remove("PASSWORDWIFI");

        if (preferences.getString("SSIDWIFI", "").length() == 0 &&
            preferences.getString("PASSWORDWIFI", "").length() == 0) {
            Serial.println("Credenciales WiFi borradas correctamente");
            delay(2000);
        } else {
            Serial.println("Error al borrar credenciales WiFi");
        }

        preferences.end();


        Serial.println("Dispositivo listo para recibir nuevas credenciales");
    }

    lastState = currentState;
}