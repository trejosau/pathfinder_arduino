#include "mq4_manager.h"

MQ4Manager::MQ4Manager() : rawValue(0), voltage(0.0), ppmCH4(0.0) {}

void MQ4Manager::begin() {
    pinMode(MQ4_PIN, INPUT);
}

void MQ4Manager::update() {
    rawValue = analogRead(MQ4_PIN);
    voltage = rawValue * (3.3 / 4095.0); // Conversión de ADC a voltaje en ESP32

    ppmCH4 = calculatePPM(rawValue);

    Serial.print("MQ-4 Raw Value: ");
    Serial.print(rawValue);
    Serial.print(" | Voltage: ");
    Serial.print(voltage);
    Serial.print("V | CH4 PPM: ");
    Serial.println(ppmCH4);
}

float MQ4Manager::calculatePPM(int rawValue) {
    // Aquí puedes definir la conversión adecuada según la curva de respuesta del sensor MQ-4
    return rawValue * 0.1;  // Ajusta este factor según la calibración
}
