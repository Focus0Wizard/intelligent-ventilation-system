
#include <WiFi.h>
#include <PubSubClient.h>
#include "TemperatureSensorController.h"
#include "SensorStates.h"

// Configuración de WiFi
const char *ssid = "nombre_red";
const char *password = "contrasena_red";

// Configuración de AWS IoT
const char *awsEndpoint = "xxxxxxxxxx-ats.iot.us-west-2.amazonaws.com";
const int awsPort = 8883;
const char *topic = "invernadero/temperatura";

TemperatureSensorController sensorController(awsEndpoint, awsPort, topic);

void setup()
{
    Serial.begin(115200);

    // Conectar a WiFi
    sensorController.connectToWiFi(ssid, password);

    // Conectar a AWS IoT
    sensorController.connectToAWS();
}

void loop()
{
    // Simulación de lectura de temperatura
    float temperatura = random(15, 50) + 0.1 * random(0, 10);

    // Determinar estado del sensor
    if (temperatura < 20.0)
    {
        sensorController.setState(new SensorTemperaturaBaja());
    }
    else if (temperatura >= 20.0 && temperatura < 25.0)
    {
        sensorController.setState(new SensorTemperaturaMedia());
    }
    else
    {
        sensorController.setState(new SensorTemperaturaAlta());
    }

    // Aplicar el estado y publicar la temperatura
    sensorController.applyState();
    sensorController.publishTemperature(temperatura);

    delay(5000);
}
