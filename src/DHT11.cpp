#include "DHT11.h"

#define DHT11_MIN_SAMPLING_PERIOD 2000 // Tiempo mínimo entre lecturas (2 segundos)

DHT11::DHT11(uint8_t pin) {
  this->pin = pin;
  lastReadTime = 0;
  temperature = 0.0;
  humidity = 0.0;
}

void DHT11::begin() {
  pinMode(pin, INPUT_PULLUP);
  delay(1000); // Dar tiempo al sensor para estabilizarse

  // Realizar una lectura inicial
  read();
}

bool DHT11::read() {
  // Verificar si ha pasado suficiente tiempo desde la última lectura
  unsigned long currentTime = millis();
  if (currentTime - lastReadTime < DHT11_MIN_SAMPLING_PERIOD) {
    return false;
  }
  lastReadTime = currentTime;

  // Iniciar la comunicación con el sensor
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delay(18);  // Al menos 18ms para wake-up

  // Cambiar a recepción
  digitalWrite(pin, HIGH);
  delayMicroseconds(40);
  pinMode(pin, INPUT_PULLUP);

  // Comprobar respuesta del sensor
  if (digitalRead(pin) == HIGH) return false;
  delayMicroseconds(80);
  if (digitalRead(pin) == LOW) return false;
  delayMicroseconds(80);

  // Leer los datos
  if (readData()) {
    // Actualizar valores si la lectura es exitosa
    humidity = data[0] + data[1] * 0.1;
    temperature = data[2] + data[3] * 0.1;
    return true;
  }
  return false;
}

bool DHT11::readData() {
  // Leer 5 bytes: humedad (entero), humedad (decimal), temperatura (entero), temperatura (decimal), checksum
  for (int i = 0; i < 5; i++) {
    data[i] = readByte();
  }

  // Verificar checksum
  uint8_t checksum = data[0] + data[1] + data[2] + data[3];
  if (checksum != data[4]) {
    return false;
  }

  return true;
}

uint8_t DHT11::readByte() {
  uint8_t result = 0;

  for (int i = 0; i < 8; i++) {
    // Esperar a que la línea suba (fin del bit anterior)
    while (digitalRead(pin) == LOW);

    // Medir duración del pulso HIGH
    unsigned long startTime = micros();
    while (digitalRead(pin) == HIGH);
    unsigned long duration = micros() - startTime;

    // Si el pulso dura más de 50µs, es un '1'
    if (duration > 40) {
      result |= (1 << (7 - i));
    }
  }

  return result;
}

float DHT11::getTemperature() {
  return temperature;
}

float DHT11::getHumidity() {
  return humidity;
}