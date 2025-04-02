#include "mq7_manager.h"

MQ7Manager::MQ7Manager() : rawValue(0), voltage(0), ppmCO(0) {}

void MQ7Manager::begin() {
    pinMode(MQ7_PIN, INPUT);
}

void MQ7Manager::update() {
    rawValue = analogRead(MQ7_PIN);
    voltage = rawValue * (3.3 / 4095.0);  // Conversión a voltaje en ESP32 (3.3V referencia)

    // Conversión básica a PPM (dependerá de calibración específica)
    ppmCO = (voltage - 0.1) * 100.0;

    Serial.print("MQ-7 Lectura: ");
    Serial.print(rawValue);
    Serial.print(" | Voltaje: ");
    Serial.print(voltage, 3);
    Serial.print("V | CO estimado: ");
    Serial.print(ppmCO);
    Serial.println(" ppm");
}
