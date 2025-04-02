// ButtonManager.cpp
#include "button_manager.h"

ButtonManager::ButtonManager(uint8_t pin) : pin(pin), lastState(HIGH), currentState(HIGH), pressStartTime(0), messageShown(false) {}

void ButtonManager::begin() {
    pinMode(pin, INPUT_PULLUP);
    preferences.clear();
}

void ButtonManager::update() {
    currentState = digitalRead(pin);
    if (isPressed() && (millis() - pressStartTime >= 3000) && !messageShown) {
        Serial.println("Suelta el botón");

        // Borrar de la memoria flash las credenciales SSID y password
        preferences.remove("SSID");
        preferences.remove("password");

        Serial.println("Credenciales borradas de la memoria flash");

        messageShown = true;
    }
}

bool ButtonManager::isPressed() {
    return currentState == LOW;
}


unsigned long ButtonManager::getPressDuration() {
    if (isPressed()) {
        return millis() - pressStartTime;
    }
    return 0;
}
