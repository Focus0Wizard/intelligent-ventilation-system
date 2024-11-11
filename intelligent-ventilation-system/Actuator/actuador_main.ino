#include <WiFi.h>
#include <PubSubClient.h>
#include "ServoController.h"
#include "ServoStates.h"

// Configuración de WiFi
const char *ssid = "nombre_red";
const char *password = "contrasena_red";

// Configuración de AWS IoT
const char *awsEndpoint = "xxxxxxxxxx-ats.iot.us-west-2.amazonaws.com";
const int awsPort = 8883;
const char *topic = "invernadero/comandos";

WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);
ServoController servoController;

// Función de callback para manejar los mensajes recibidos de AWS IoT
void callback(char *topic, byte *payload, unsigned int length)
{
    String message;
    for (int i = 0; i < length; i++)
    {
        message += (char)payload[i]; // Convertir el payload a un String
    }
    Serial.println("Mensaje recibido: " + message);

    // Determinar el estado del servo en función del mensaje recibido
    if (message == "0")
    {
        servoController.setState(new ServoApagado()); // Estado de apagado
    }
    else if (message == "1")
    {
        servoController.setState(new ServoVelocidadBaja()); // Estado de velocidad baja
    }
    else if (message == "2")
    {
        servoController.setState(new ServoVelocidadMedia()); // Estado de velocidad media
    }
    else if (message == "3")
    {
        servoController.setState(new ServoVelocidadAlta()); // Estado de velocidad alta
    }

    // Aplicar el estado del servo
    servoController.applyState();
}

void setup()
{
    Serial.begin(115200);

    // Conectar a WiFi
    Serial.print("Conectando a WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print("."); // Mostrar puntos mientras se conecta
    }
    Serial.println("Conectado");

    // Configurar la conexión segura a AWS IoT
    wifiClient.setCACert(ca_cert);          // Establecer el certificado raíz
    wifiClient.setCertificate(client_cert); // Establecer el certificado del cliente
    wifiClient.setPrivateKey(priv_key);     // Establecer la clave privada

    client.setServer(awsEndpoint, awsPort); // Configurar el servidor de AWS IoT
    client.setCallback(callback);           // Establecer la función de callback

    // Conectar a AWS IoT
    Serial.print("Conectando a AWS IoT...");
    while (!client.connected())
    {
        if (client.connect("ESP32ActuatorClient"))
        {
            Serial.println("Conectado a AWS IoT");
            client.subscribe(topic); // Suscribirse al tópico de comandos
        }
        else
        {
            Serial.print("Fallo, rc=");
            Serial.print(client.state());
            Serial.println(" Reintentando en 5 segundos");
            delay(5000); // Esperar 5 segundos antes de reintentar
        }
    }
}

void loop()
{
    // Verificar si la conexión a AWS IoT se ha perdido
    if (!client.connected())
    {
        while (!client.connected())
        {
            Serial.print("Reconectando a AWS IoT...");
            if (client.connect("ESP32ActuatorClient"))
            {
                Serial.println("Reconectado");
                client.subscribe(topic); // Suscribirse nuevamente al tópico
            }
            else
            {
                Serial.print("Fallo, rc=");
                Serial.print(client.state());
                Serial.println(" Reintentando en 5 segundos");
                delay(5000); // Esperar 5 segundos antes de reintentar
            }
        }
    }
    client.loop(); // Mantener el cliente MQTT en funcionamiento
}
