#ifndef GPS_MANAGER_H
#define GPS_MANAGER_H

#include <Arduino.h>
#include <TinyGPSPlus.h>

class GPSManager {
public:
    /**
     * Constructor.
     * @param serialPort: Referencia al HardwareSerial que se usará para el GPS.
     * @param rxPin: Pin RX para el GPS.
     * @param txPin: Pin TX para el GPS.
     */
    GPSManager(HardwareSerial &serialPort, uint8_t rxPin, uint8_t txPin);

    /**
     * Inicializa el puerto serial del GPS.
     */
    void begin();

    /**
     * Lee los datos del GPS y actualiza la información.
     */
    void update();

    /**
     * Indica si se actualizó la ubicación en esta iteración.
     */
    bool locationUpdated();

    /**
     * Retorna la latitud actual.
     */
    double getLatitude();

    /**
     * Retorna la longitud actual.
     */
    double getLongitude();

private:
    HardwareSerial &gpsSerial;
    uint8_t rxPin;
    uint8_t txPin;
    TinyGPSPlus gps;
};

#endif
