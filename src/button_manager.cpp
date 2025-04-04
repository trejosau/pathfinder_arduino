// ButtonManager.cpp
#include "button_manager.h"
#define BUZZER_PIN 25
ButtonManager::ButtonManager(uint8_t pin) : pin(pin), lastState(HIGH), currentState(HIGH), pressStartTime(0), messageShown(false) {}

void ButtonManager::begin() {
    pinMode(pin, INPUT_PULLUP);

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
        digitalWrite(BUZZER_PIN, LOW);
    }

    lastState = currentState;
}