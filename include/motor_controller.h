#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>

// Definición de pines para el L298N
#define MOTOR_ENA 14  // PWM para Motor A
#define MOTOR_IN1 5   // Dirección 1 Motor A
#define MOTOR_IN2 18  // Dirección 2 Motor A
#define MOTOR_IN3 19  // Dirección 1 Motor B
#define MOTOR_IN4 21  // Dirección 2 Motor B
#define MOTOR_ENB 12  // PWM para Motor B

// Definición de direcciones
enum Direction {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    STOP
};

class MotorController {
public:
    MotorController();
    void begin();
    
    // Control de velocidad (0-255)
    void setSpeedA(uint8_t speed);
    void setSpeedB(uint8_t speed);
    void setSpeed(uint8_t speed);
    
    // Control de dirección
    void setDirection(Direction direction);
    
    // Movimientos específicos
    void moveForward(uint8_t speed);
    void moveBackward(uint8_t speed);
    void turnLeft(uint8_t speed);
    void turnRight(uint8_t speed);
    void stop();

private:
    uint8_t currentSpeedA;
    uint8_t currentSpeedB;
    Direction currentDirection;
    
    // Control de motores individuales
    void controlMotorA(bool in1State, bool in2State);
    void controlMotorB(bool in3State, bool in4State);
};

#endif