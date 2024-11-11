#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "ServoController.h"
#include "ServoStates.h"

// Replace with your network credentials
const char *WIFI_SSID = "LABO17";
const char *WIFI_PASS = "catolica17";

// Replace with your MQTT broker details
const char *MQTT_BROKER = "YOUR_ENDPOINT";
const int MQTT_PORT = 8883;
const char *CLIENT_ID = "ESP-32";

// Replace with your think shadow details
const char *UPDATE_TOPIC = "$aws/things/myThing/shadow/update";             // publish
const char *UPDATE_DELTA_TOPIC = "$aws/things/myThing/shadow/update/delta"; // subscribe

const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
-----END RSA PRIVATE KEY-----
)KEY";

WiFiClientSecure wiFiClient;
PubSubClient client(wiFiClient);

StaticJsonDocument<JSON_OBJECT_SIZE(64)> inputDoc;

ServoController servoController;

// Callback function to handle messages received from the subscribed topic
void callback(char *topic, byte *payload, unsigned int length)
{
    String message;
    for (int i = 0; i < length; i++)
    {
        message += String((char)payload[i]);
    }
    Serial.println("Message from topic " + String(topic) + ":" + message);

    DeserializationError err = deserializeJson(inputDoc, payload);
    if (!err)
    {
        if (String(topic) == UPDATE_DELTA_TOPIC)
        {
            int command = inputDoc["state"]["builtInLed"].as<int>();

            // Determinar estado del servo en función del mensaje recibido
            if (command == 0)
            {
                servoController.setState(new ServoApagado());
            }
            else if (command == 1)
            {
                servoController.setState(new ServoVelocidadBaja());
            }
            else if (command == 2)
            {
                servoController.setState(new ServoVelocidadMedia());
            }
            else if (command == 3)
            {
                servoController.setState(new ServoVelocidadAlta());
            }

            // Aplicar el estado del servo
            servoController.applyState();
        }
    }
}

void setupWiFi()
{
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("Connected to WiFi. IP address: ");
    Serial.println(WiFi.localIP());
}

void setup()
{
    Serial.begin(115200);
    setupWiFi();

    wiFiClient.setCACert(AMAZON_ROOT_CA1);
    wiFiClient.setCertificate(CERTIFICATE);
    wiFiClient.setPrivateKey(PRIVATE_KEY);

    client.setServer(MQTT_BROKER, MQTT_PORT);
    client.setCallback(callback);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();
}
