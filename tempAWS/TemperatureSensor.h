#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <DHT.h>

class TemperatureSensor
{
private:
    DHT dht;
    int temperatureState;

public:
    TemperatureSensor(int pin, int type) : dht(pin, type), temperatureState(-1) {}

    void begin()
    {
        dht.begin();
    }

    int readTemperatureState()
    {
        float temp = dht.readTemperature();
        if (!isnan(temp))
        {
            if (temp > 0 && temp < 20)
            {
                return 0;
            }
            else if (temp >= 20 && temp < 25)
            {
                return 1;
            }
            else if (temp >= 25 && temp < 50)
            {
                return 2;
            }
        }
        return temperatureState;
    }

    bool hasStateChanged(int newTemperatureState)
    {
        if (newTemperatureState != temperatureState)
        {
            temperatureState = newTemperatureState;
            return true;
        }
        return false;
    }

    void printTemperature()
    {
        float temp = dht.readTemperature();
        Serial.print("Temperatura actual: ");
        Serial.println(temp);
    }
};

#endif
