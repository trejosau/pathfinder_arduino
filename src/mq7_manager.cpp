#include "mq7_manager.h"

MQ7Manager::MQ7Manager() : rawValue(0), voltage(0.0), ppmCO(0.0) {}

void MQ7Manager::begin() {
    pinMode(MQ7_PIN, INPUT);
}

void MQ7Manager::update() {
    rawValue = analogRead(MQ7_PIN);
    // Convertimos la lectura ADC a voltaje usando 3.3V como referencia
    voltage = rawValue * (3.3 / 4095.0);
    // Factor de conversión provisional; deberás calibrarlo según tus pruebas
    ppmCO = rawValue * 0.1;
}
