#ifndef SERVOCONTROLLER_H
#define SERVOCONTROLLER_H

#include "DeviceState.h"

class ServoController
{
private:
    DeviceState *currentState;

public:
    ServoController() : currentState(nullptr) {}

    void setState(DeviceState *state)
    {
        if (currentState)
            delete currentState;
        currentState = state;
    }

    void applyState()
    {
        if (currentState)
            currentState->handleRequest();
    }

    ~ServoController()
    {
        if (currentState)
            delete currentState;
    }
};

#endif
