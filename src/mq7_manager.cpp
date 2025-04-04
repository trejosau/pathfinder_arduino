#include "mq7_manager.h"

MQ7Manager::MQ7Manager() : rawValue(0), voltage(0.0), ppmCO(0.0) {}

void MQ7Manager::begin() {
    pinMode(MQ7_PIN, INPUT);
}

void MQ7Manager::update() {
    rawValue = analogRead(MQ7_PIN);


}
