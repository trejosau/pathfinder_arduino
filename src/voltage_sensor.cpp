#include <Arduino.h>
#include "voltage_sensor.h"

VoltageSensor::VoltageSensor(int pin) {
    sensorPin = pin;
}

void VoltageSensor::begin() {
    analogReadResolution(12);  // Asegura resolución de 12 bits
}

float VoltageSensor::getVoltage() {
    int rawValue = analogRead(sensorPin);
    float vOut = (rawValue / 4095.0) * vCC;  // ← corregido aquí
    float vIn = vOut * factor;
    return vIn;
}

float VoltageSensor::getBatteryPercentage() {
    float vIn = getVoltage();
    float percentage = ((vIn - minBatteryVoltage) / (maxBatteryVoltage - minBatteryVoltage)) * 100.0;
    if (percentage > 100.0) percentage = 100.0;
    if (percentage < 0.0) percentage = 0.0;
    return percentage;
}
