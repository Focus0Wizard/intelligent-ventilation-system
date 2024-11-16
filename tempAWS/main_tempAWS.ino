#include "WiFiManager.h"
#include "MQTTManager.h"
#include "TemperatureSensor.h"

// Network credentials
const char *WIFI_SSID = "JhonnP";
const char *WIFI_PASS = "alberto1234";

// MQTT broker details
const char *MQTT_BROKER = "abkb9nclg5hh-ats.iot.us-east-1.amazonaws.com";
const int MQTT_PORT = 8883;
const char *CLIENT_ID = "ESP32_SENSOR";

// AWS certificates
extern const char AMAZON_ROOT_CA1[];
extern const char CERTIFICATE[];
extern const char PRIVATE_KEY[];

// Objects
WiFiManager wifiManager;
MQTTManager mqttManager(MQTT_BROKER, MQTT_PORT, CLIENT_ID, "$aws/things/TempVentControl/shadow/update");
TemperatureSensor temperatureSensor(4, DHT11);

void setup()
{
    Serial.begin(115200);
    temperatureSensor.begin();
    wifiManager.connect(WIFI_SSID, WIFI_PASS);
    mqttManager.setupTLS(AMAZON_ROOT_CA1, CERTIFICATE, PRIVATE_KEY);
    mqttManager.connect();
}

void loop()
{
    mqttManager.loop();

    int newTemperatureState = temperatureSensor.readTemperatureState();
    if (temperatureSensor.hasStateChanged(newTemperatureState))
    {
        mqttManager.reportShadowState(newTemperatureState);
    }

    temperatureSensor.printTemperature();
    delay(2000);
}
