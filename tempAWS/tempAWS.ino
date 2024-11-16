#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// Network credentials
const char* WIFI_SSID = "JhonnP";
const char* WIFI_PASS = "alberto1234";

// MQTT broker details
const char* MQTT_BROKER = "abkb9nclg5hh-ats.iot.us-east-1.amazonaws.com";
const int MQTT_PORT = 8883;
const char* CLIENT_ID = "ESP32_SENSOR";

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
MIIDWTCCAkGgAwIBAgIUOfux9mhJ1bwiA2P/gAlrDkSuWPEwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI0MTEwNzAwNTUw
MFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL7BQyPxHdxPRIgF6kEn
2rrmb5vMc8h83pPSOanvhud/vsxBnWuxQy2edj0duLZSJFR+12YKnprM0k+uaEwd
ohvZR8wGYgNFQOMZt0Teqw7ZZxA6Wm1oTm9abpeubH1nOClVTGQaHBwLyC5YSCKU
EmvJam7xtd1vyEd8cNamLUQyrj1LKcwQLPSOtaoEgTgR/edVYvnYUK+EOBJQUMlk
ULCoTLB3xCaZ9fOZtSajxp6hhwi/lgGm/A2cf6VyyWXrA2gtweDClJZnehVLR7oO
tVMb8gPTHysJqKRoPwtmM4uuewTjBLFtRGzdSOx5a/4xfkHyf14Kj1l6HAStGKs0
bCsCAwEAAaNgMF4wHwYDVR0jBBgwFoAUKbynf5unhu4HGubQOWbwnH1Ze6owHQYD
VR0OBBYEFGZXb6CMmsxJ+T6n3Jd2Au7lSx6zMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAjZDTW1pXHl6oiUoBmYBApnG25
aywsjTgvNv/eiktG6jZVCXymqcSlU9gl9LOZMbMcyV4amFDYvjyvciKtpbHVmQno
agMYbGEEASLD9eooJikPTJujrA+iT5traJC2EnALViZ8edaB4oEkvZnLdYMeiQ+e
BNM3Z1bj7uuuUglCPFUAD2m5mov9T1kpB6cg9nXQcUX8G1PxF/Yk0NBfsHK7hUFV
x6f+w0xymUpublbBS7I1n97ahX6YFY2eMal5QsqLRTtFZN+FnBUV8GO+8X0PcmXE
k8c5J6SBjSJVgAk25AwBCMUE9XMjCIdRv46ohqqmkV6yp1B3KcJPFlUK2Ps9
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAvsFDI/Ed3E9EiAXqQSfauuZvm8xzyHzek9I5qe+G53++zEGd
a7FDLZ52PR24tlIkVH7XZgqemszST65oTB2iG9lHzAZiA0VA4xm3RN6rDtlnEDpa
bWhOb1pul65sfWc4KVVMZBocHAvILlhIIpQSa8lqbvG13W/IR3xw1qYtRDKuPUsp
zBAs9I61qgSBOBH951Vi+dhQr4Q4ElBQyWRQsKhMsHfEJpn185m1JqPGnqGHCL+W
Aab8DZx/pXLJZesDaC3B4MKUlmd6FUtHug61UxvyA9MfKwmopGg/C2Yzi657BOME
sW1EbN1I7Hlr/jF+QfJ/XgqPWXocBK0YqzRsKwIDAQABAoIBAHanxNnQnCB0dk6e
T07kOZzUxkwFa2gktFcDB7d7NVE/FzH7Bq78fqsbzLTLjiBrDAo7NEVXCb2E9rV1
6Y6fSD1GdPCmb8VVCL27O1Ltpz3/YaqogaNYpKGDgRN8/iMrV7XuRyxlSWLwuT6K
z6Qisyq1CS2l0NjDNbOx5oa+q52lTLG1uxYiGgdQD3fbBmRi/3535su+S54WCQl7
ERo66Vd5uPp1reSgPV9j+oDV2voV5HAJeljIgOwmjGg9KjKmAinpOwbVy/eQ3SCS
fokv8ncSAkgxhZh/LRZop1WSIXXsK1f/b9J1cL0wEukorw1AR4vmGr0h/jk2vt0U
VWK343ECgYEA8u5lyy6rjFUwgv2iXS9nTMkOfT76tdAGp/t5/uEohPGnwoQnEm5O
LRACMxsEI2FL0S890w3DiFr1fCJzRyXSyIOPY4NaeBWJNgbrua85pnJOF6x1Ilv6
fGMix7QWt+pVqBHpqCxmwiZNi7vaqVdNHtiYoTk/Y6RMqLSzoUhUwi0CgYEAyQRN
ce6iB50umurp+FSeKIMYgdvwwx+vJzh1UcLH9mIUkbuW9lsHSQkpTJZNnOKGNjh4
PTFJVhqFzY0cdKZNbUt1od8nT9m/GfXVm2hY30I81SPv8VEYaB23khqZ7diMWP7W
f1XOb5ITmbbVV7IcpZFUgDdTq9g2YTccmtj81rcCgYEA24MFolJamuTDgCnVk+Ro
sF6g5BWLj8wYp6O5+dDUv0RpdX/amnHn3BoyEqHf3GRnkO7iK5qtS8H9Pp0Wk3ye
+aa/Y4lmr0mugY5BZLjsG/X/kK657UE1lz4xSEj3mlp56fO9scPlWpwnyBMII6qK
Okw3pr9HAy+MQYl/I0ZrqEUCgYAb9Gc4wvDxkiBbFvQ2vr3H8INF++o9mHhpQea+
2hGo5/boVWgnFn2q4IVHy1c7zBzSCYRfLi1+te2JqmgcqMv3uRjAzCHRbvyuhYuR
My+4zKNdGMRTkO2N/SP8Q4rIkubhsX7u8mXlCjGDnPOKYJCvTcislAUkzsqxNlLa
DfcLfQKBgQCWGu/F0C/hsE3YTGiJwpuutF+WZHBi9JyDc3P5dgcOkuY2v2QkdEKs
W4L2oufAwH1Ua6OUE5hkYixduCBTSipj9TFhlIcJwqoI7nf6/jYzpV/JfJz46wkk
TlPJNS9K2r9Cq8xV3XJnTXmbjZeY2yErS19gUeyd7a1b1H9hvtHyOQ==
-----END RSA PRIVATE KEY-----
)KEY";

// Shadow details
const char* UPDATE_TOPIC = "$aws/things/TempVentControl/shadow/update";

// Pins
#define DHTPIN 4
#define DHTTYPE DHT11

class WiFiManager {
public:
    void connect(const char* ssid, const char* password) {
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);

        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println();
        Serial.print("Connected to WiFi. IP address: ");
        Serial.println(WiFi.localIP());
    }
};

class MQTTManager {
private:
    WiFiClientSecure wifiClient;
    PubSubClient client;
    const char* broker;
    int port;
    const char* clientId;
    const char* updateTopic;
    StaticJsonDocument<JSON_OBJECT_SIZE(64)> outputDoc;
    char outputBuffer[256];

public:
    MQTTManager(const char* broker, int port, const char* clientId, const char* updateTopic)
        : client(wifiClient), broker(broker), port(port), clientId(clientId), updateTopic(updateTopic) {}

    void setupTLS(const char* rootCA, const char* certificate, const char* privateKey) {
        wifiClient.setCACert(rootCA);
        wifiClient.setCertificate(certificate);
        wifiClient.setPrivateKey(privateKey);
    }

    void connect() {
        client.setServer(broker, port);
    }

    void reconnect() {
        while (!client.connected()) {
            Serial.print("Attempting MQTT connection...");
            if (client.connect(clientId)) {
                Serial.println("connected");
                delay(100);
                reportShadowState(0); // Send initial state
            } else {
                Serial.print("failed, rc=");
                Serial.print(client.state());
                Serial.println(" trying again in 5 seconds");
                delay(5000);
            }
        }
    }

    void loop() {
        if (!client.connected()) {
            reconnect();
        }
        client.loop();
    }

    void reportShadowState(int temperatureState) {
        outputDoc["state"]["desired"]["temperatureState"] = temperatureState;
        serializeJson(outputDoc, outputBuffer);
        client.publish(updateTopic, outputBuffer);
    }
};

class TemperatureSensor {
private:
    DHT dht;
    int temperatureState;

public:
    TemperatureSensor(int pin, int type) : dht(pin, type), temperatureState(-1) {}

    void begin() {
        dht.begin();
    }

    int readTemperatureState() {
        float temp = dht.readTemperature();
        if (!isnan(temp)) {
            if (temp > 0 && temp < 20) {
                return 0;
            } else if (temp >= 20 && temp < 25) {
                return 1;
            } else if (temp >= 25 && temp < 50) {
                return 2;
            }
        }
        return temperatureState;
    }

    bool hasStateChanged(int newTemperatureState) {
        if (newTemperatureState != temperatureState) {
            temperatureState = newTemperatureState;
            return true;
        }
        return false;
    }

    void printTemperature() {
        float temp = dht.readTemperature();
        Serial.print("Temperatura actual: ");
        Serial.println(temp);
    }
};

// Objects
WiFiManager wifiManager;
MQTTManager mqttManager(MQTT_BROKER, MQTT_PORT, CLIENT_ID, UPDATE_TOPIC);
TemperatureSensor temperatureSensor(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(115200);
    temperatureSensor.begin();
    wifiManager.connect(WIFI_SSID, WIFI_PASS);
    mqttManager.setupTLS(AMAZON_ROOT_CA1, CERTIFICATE, PRIVATE_KEY);
    mqttManager.connect();
}

void loop() {
    mqttManager.loop();

    int newTemperatureState = temperatureSensor.readTemperatureState();
    if (temperatureSensor.hasStateChanged(newTemperatureState)) {
        mqttManager.reportShadowState(newTemperatureState);
    }

    temperatureSensor.printTemperature();
    delay(2000);
}
