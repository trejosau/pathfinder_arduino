#include "dht_manager.h"

DHTManager::DHTManager(uint8_t pin) : pin(pin), dht(pin, DHT22) {}

void DHTManager::begin() {
    dht.begin();
}

void DHTManager::update() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Error al leer el sensor DHT22");
        return;
    }

    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.print(" °C, Humedad: ");
    Serial.print(humidity);
    Serial.println(" %");
}
