// ButtonManager.cpp
#include "button_manager.h"

ButtonManager::ButtonManager(uint8_t pin) : pin(pin), lastState(HIGH), currentState(HIGH), pressStartTime(0), messageShown(false) {}

void ButtonManager::begin() {
    pinMode(pin, INPUT_PULLUP);
    preferences.begin("button-config", false);
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
        Serial.println("Suelta el botón");

        // Borrar credenciales WiFi
        preferences.begin("wifi-config", false);
        preferences.remove("SSIDWIFI");
        preferences.remove("PASSWORDWIFI");
        preferences.end();

        Serial.println("Credenciales borradas de la memoria flash");
        messageShown = true;
    }

    lastState = currentState;
}