#ifndef VOLTAGE_SENSOR_H
#define VOLTAGE_SENSOR_H

class VoltageSensor {
public:
    VoltageSensor(int pin = 35);  // Pin A3 = GPIO 39 en ESP32
    void begin();
    float getVoltage();           // Voltaje real de la batería
    float getBatteryPercentage(); // Porcentaje basado en umbrales

private:
    int sensorPin;
    const float factor = 5.128;
    const float vCC = 5.00;
    const float minBatteryVoltage = 4.0;
    const float maxBatteryVoltage = 5.5;
};

#endif
