#include "incline_sensor.h"
#include "TokyoDrift.h"



InclineSensor::InclineSensor(uint8_t pin) {
    this->pin = pin;
}

void InclineSensor::begin() {
    // Configuramos el pin como entrada con resistencia pull-up, ya que el sensor solo tiene 3 pines.
    pinMode(pin, INPUT_PULLUP);
}

bool InclineSensor::isInclined() {
    // Suponiendo que el sensor devuelve LOW cuando se detecta la inclinación.
    return (digitalRead(pin) == LOW);
}
