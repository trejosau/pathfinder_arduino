#include "voltage_sensor.h"

VoltageSensor::VoltageSensor() : rawValue(0), voltage(0.0), calibrationFactor(1.0) {}

void VoltageSensor::begin() {
    pinMode(VOLTAGE_SENSOR_PIN, INPUT);

    calibrationFactor = 5.28;

    Serial.println("Sensor de voltaje inicializado. Calibración: " + String(calibrationFactor));
}

void VoltageSensor::update() {
    // Realizar múltiples lecturas para estabilidad
    rawValue = 0;
    for (int i = 0; i < 10; i++) {
        rawValue += analogRead(VOLTAGE_SENSOR_PIN);
        delay(1);
    }
    rawValue /= 10; // Promedio de 10 lecturas

    // Calcular voltaje medido con el ADC de 12 bits (0-4095) del ESP32
    float measuredVoltage = (rawValue * referenceVoltage) / 4095.0;

    // Aplicar el factor de calibración para obtener el voltaje real de la batería
    voltage = measuredVoltage * calibrationFactor;

    // Imprimir valores para depuración
    Serial.print("Batería - Raw ADC: ");
    Serial.print(rawValue);
    Serial.print(" | Voltaje: ");
    Serial.print(voltage, 2);  // 2 decimales
    Serial.print("V | Porcentaje: ");
    Serial.print(getBatteryPercentage());
    Serial.println("%");
}

float VoltageSensor::getVoltage() {
    return voltage;
}

float VoltageSensor::getRawValue() {
    return rawValue;
}

float VoltageSensor::getBatteryPercentage() {
    // Calcula el porcentaje de batería basado en los umbrales definidos.
    float percentage = ((voltage - minBatteryVoltage) / (maxBatteryVoltage - minBatteryVoltage)) * 100.0;

    // Limitar el rango entre 0 y 100%
    if (percentage > 100.0) percentage = 100.0;
    if (percentage < 0.0) percentage = 0.0;

    return percentage;
}
