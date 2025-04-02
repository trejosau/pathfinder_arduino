#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>

class ButtonManager {
public:
    ButtonManager(uint8_t pin);
    void begin();
    void update();
    bool isPressed();
    unsigned long getPressDuration();

private:
    uint8_t pin;
    bool lastState;
    bool currentState;
    unsigned long pressStartTime;
    bool messageShown;
    Preferences preferences;
};

#endif
