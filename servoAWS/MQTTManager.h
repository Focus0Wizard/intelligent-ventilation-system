#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

class MQTTManager
{
private:
    WiFiClientSecure wifiClient;
    PubSubClient client;
    const char *broker;
    int port;
    const char *clientId;
    const char *updateTopic;
    StaticJsonDocument<JSON_OBJECT_SIZE(64)> outputDoc;
    char outputBuffer[256];

public:
    MQTTManager(const char *broker, int port, const char *clientId, const char *updateTopic)
        : client(wifiClient), broker(broker), port(port), clientId(clientId), updateTopic(updateTopic) {}

    void setupTLS(const char *rootCA, const char *certificate, const char *privateKey)
    {
        wifiClient.setCACert(rootCA);
        wifiClient.setCertificate(certificate);
        wifiClient.setPrivateKey(privateKey);
    }

    void connect()
    {
        client.setServer(broker, port);
    }

    void reconnect()
    {
        while (!client.connected())
        {
            Serial.print("Attempting MQTT connection...");
            if (client.connect(clientId))
            {
                Serial.println("connected");
                delay(100);
                reportShadowState(0); // Send initial state
            }
            else
            {
                Serial.print("failed, rc=");
                Serial.print(client.state());
                Serial.println(" trying again in 5 seconds");
                delay(5000);
            }
        }
    }

    void loop()
    {
        if (!client.connected())
        {
            reconnect();
        }
        client.loop();
    }

    void reportShadowState(int temperatureState)
    {
        outputDoc["state"]["desired"]["temperatureState"] = temperatureState;
        serializeJson(outputDoc, outputBuffer);
        client.publish(updateTopic, outputBuffer);
    }
};

#endif
