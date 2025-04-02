#ifndef VOLTAGE_SENSOR_H
#define VOLTAGE_SENSOR_H

#include <Arduino.h>

#define VOLTAGE_SENSOR_PIN 34    // Pin donde está conectado el sensor FZ0430

class VoltageSensor {
public:
    VoltageSensor();
    void begin();
    void update();
    float getVoltage();           // Obtener el voltaje actual
    float getRawValue();          // Obtener el valor raw del ADC
    float getBatteryPercentage(); // Obtener el porcentaje de batería

private:
    int rawValue;                 // Valor bruto del ADC
    float voltage;                // Voltaje calculado
    const float referenceVoltage = 3.3;  // Voltaje de referencia del ESP32
    const float maxBatteryVoltage = 4.8;   // Voltaje máximo de las 4 pilas juntas
    const float minBatteryVoltage = 3.6;   // Voltaje mínimo funcional (ajustar según tus pilas)
    float calibrationFactor;      // Factor de calibración para ajustar la lectura
};

#endif
