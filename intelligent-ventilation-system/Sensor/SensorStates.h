#ifndef SENSORSTATES_H
#define SENSORSTATES_H

#include "DeviceState.h"
#include <Arduino.h>

class SensorTemperaturaBaja : public DeviceState
{
public:
    void handleRequest() override
    {
        Serial.println("Temperatura Baja: Activar calefacción");
    }
};

class SensorTemperaturaMedia : public DeviceState
{
public:
    void handleRequest() override
    {
        Serial.println("Temperatura Media: Estado Normal");
    }
};

class SensorTemperaturaAlta : public DeviceState
{
public:
    void handleRequest() override
    {
        Serial.println("Temperatura Alta: Activar ventilación");
    }
};

#endif 
