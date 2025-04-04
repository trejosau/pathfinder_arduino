#ifndef VOLTAGE_SENSOR_H
#define VOLTAGE_SENSOR_H

#include <Arduino.h>

#define VOLTAGE_SENSOR_PIN 34    // Pin ADC donde se lee la salida del sensor de voltaje

class VoltageSensor {
public:
    VoltageSensor();
    void begin();
    void update();
    float getVoltage();           // Devuelve el voltaje medido (ya calibrado)
    float getRawValue();          // Devuelve el valor bruto del ADC
    float getBatteryPercentage(); // Devuelve el porcentaje de batería calculado

private:
    int rawValue;                 // Valor bruto del ADC
    float voltage;                // Voltaje calculado (después de aplicar la calibración)
    const float referenceVoltage = 3.3;  // Voltaje de referencia del ESP32
    const float maxBatteryVoltage = 4.8;   // Voltaje máximo esperado de la batería (ajusta según tu sistema)
    const float minBatteryVoltage = 3.6;   // Voltaje mínimo funcional para la batería (ajusta según tus pilas)
    float calibrationFactor;      // Factor de calibración para ajustar la lectura (determinado experimentalmente)
};

#endif
