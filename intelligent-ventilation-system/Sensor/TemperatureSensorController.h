#ifndef TEMPERATURESENSORCONTROLLER_H
#define TEMPERATURESENSORCONTROLLER_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "DeviceState.h"

class TemperatureSensorController
{
private:
    DeviceState *currentState;
    WiFiClientSecure wifiClient;
    PubSubClient client;
    const char *topic;

public:
    TemperatureSensorController(const char *awsEndpoint, int awsPort, const char *topicName)
        : client(wifiClient), topic(topicName), currentState(nullptr)
    {
        client.setServer(awsEndpoint, awsPort);
    }

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

    void connectToWiFi(const char *ssid, const char *password);
    void connectToAWS();
    void publishTemperature(float temperatura);

    ~TemperatureSensorController()
    {
        if (currentState)
            delete currentState;
    }
};

#endif
