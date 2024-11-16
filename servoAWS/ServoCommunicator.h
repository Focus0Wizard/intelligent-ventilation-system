#ifndef SERVO_COMMUNICATOR_H
#define SERVO_COMMUNICATOR_H

#include "MQTTManager.h"
#include "ServoMotor.h"

class ServoCommunicator
{
private:
    MQTTManager &mqttManager;
    ServoMotor &servoMotor;

public:
    ServoCommunicator(MQTTManager &mqttManager, ServoMotor &servoMotor)
        : mqttManager(mqttManager), servoMotor(servoMotor) {}

    void setup()
    {
        mqttManager.connect();
    }

    void loop()
    {
        mqttManager.loop();
    }

    void handleIncomingMessage(char *topic, byte *payload, unsigned int length)
    {
        // Maneja los mensajes entrantes y actualiza el estado del servo
    }
};

#endif
