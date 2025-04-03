#include "motor_controller.h"

MotorController::MotorController() 
    : currentSpeedA(0), currentSpeedB(0), currentDirection(STOP) {
}

void MotorController::begin() {
    // Configurar pines como salidas
    pinMode(MOTOR_ENA, OUTPUT);
    pinMode(MOTOR_IN1, OUTPUT);
    pinMode(MOTOR_IN2, OUTPUT);
    pinMode(MOTOR_IN3, OUTPUT);
    pinMode(MOTOR_IN4, OUTPUT);
    pinMode(MOTOR_ENB, OUTPUT);
    
    // Inicializar en estado de parada
    stop();
    
    Serial.println("Motor Controller inicializado");
}

void MotorController::setSpeedA(uint8_t speed) {
    currentSpeedA = speed;
    ledcWrite(0, speed); // Suponiendo que usamos el canal 0 para ENA
}

void MotorController::setSpeedB(uint8_t speed) {
    currentSpeedB = speed;
    ledcWrite(1, speed); // Suponiendo que usamos el canal 1 para ENB
}

void MotorController::setSpeed(uint8_t speed) {
    setSpeedA(speed);
    setSpeedB(speed);
}

void MotorController::controlMotorA(bool in1State, bool in2State) {
    digitalWrite(MOTOR_IN1, in1State);
    digitalWrite(MOTOR_IN2, in2State);
}

void MotorController::controlMotorB(bool in3State, bool in4State) {
    digitalWrite(MOTOR_IN3, in3State);
    digitalWrite(MOTOR_IN4, in4State);
}

void MotorController::setDirection(Direction direction) {
    currentDirection = direction;
    
    switch (direction) {
        case FORWARD:
            moveForward(max(currentSpeedA, currentSpeedB));
            break;
        case BACKWARD:
            moveBackward(max(currentSpeedA, currentSpeedB));
            break;
        case LEFT:
            turnLeft(max(currentSpeedA, currentSpeedB));
            break;
        case RIGHT:
            turnRight(max(currentSpeedA, currentSpeedB));
            break;
        case STOP:
        default:
            stop();
            break;
    }
}

void MotorController::moveForward(uint8_t speed) {
    // Motor A: Adelante
    controlMotorA(HIGH, LOW);
    // Motor B: Adelante
    controlMotorB(HIGH, LOW);
    
    setSpeed(speed);
}

void MotorController::moveBackward(uint8_t speed) {
    // Motor A: Atrás
    controlMotorA(LOW, HIGH);
    // Motor B: Atrás
    controlMotorB(LOW, HIGH);
    
    setSpeed(speed);
}

void MotorController::turnLeft(uint8_t speed) {
    // Motor A: Parado o Atrás (dependiendo de si quieres giro sobre el eje o giro suave)
    controlMotorA(LOW, LOW); // Parado
    // Motor B: Adelante
    controlMotorB(HIGH, LOW);
    
    setSpeedB(speed);
}

void MotorController::turnRight(uint8_t speed) {
    // Motor A: Adelante
    controlMotorA(HIGH, LOW);
    // Motor B: Parado o Atrás
    controlMotorB(LOW, LOW); // Parado
    
    setSpeedA(speed);
}

void MotorController::stop() {
    // Parar ambos motores
    controlMotorA(LOW, LOW);
    controlMotorB(LOW, LOW);
    
    setSpeed(0);
}