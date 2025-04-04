#include "mq4_manager.h"

MQ4Manager::MQ4Manager() : rawValue(0), voltage(0.0), ppmCH4(0.0) {}

void MQ4Manager::begin() {
    pinMode(MQ4_PIN, INPUT);
}

void MQ4Manager::update() {
    rawValue = analogRead(MQ4_PIN);


}

float MQ4Manager::calculatePPM(int rawValue) {
    return rawValue * 0.1;
}
