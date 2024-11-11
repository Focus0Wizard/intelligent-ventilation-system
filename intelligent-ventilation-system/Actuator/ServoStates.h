#ifndef SERVOSTATES_H
#define SERVOSTATES_H

#include "DeviceState.h"
#include <Arduino.h>

class ServoApagado : public DeviceState
{
public:
    void handleRequest() override
    {
        Serial.println("Servo Apagado");
    }
};

class ServoVelocidadBaja : public DeviceState
{
public:
    void handleRequest() override
    {
        Serial.println("Servo en Velocidad Baja");
    }
};

class ServoVelocidadMedia : public DeviceState
{
public:
    void handleRequest() override
    {
        Serial.println("Servo en Velocidad Media");
    }
};

class ServoVelocidadAlta : public DeviceState
{
public:
    void handleRequest() override
    {
        Serial.println("Servo en Velocidad Alta");
    }
};

#endif
