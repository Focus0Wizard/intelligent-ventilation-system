#ifndef SERVO_MOTOR_H
#define SERVO_MOTOR_H

#include <ESP32Servo.h>

class ServoMotor
{
private:
    Servo servo;
    int servoPin;
    int currentAngle;
    int lastServoState;

public:
    ServoMotor(int pin) : servoPin(pin), currentAngle(90), lastServoState(-1) {}

    void setup()
    {
        servo.attach(servoPin);
        servo.write(currentAngle);
    }

    void updateFromState(int servoState)
    {
        if (servoState != lastServoState)
        {
            lastServoState = servoState;
            switch (servoState)
            {
            case 0:
                currentAngle = 90;
                break;
            case 1:
                currentAngle = 115;
                break;
            case 2:
                currentAngle = 145;
                break;
            case 3:
                currentAngle = 180;
                break;
            default:
                Serial.println("Unknown servoState");
                return;
            }
            servo.write(currentAngle);
            Serial.print("Servo updated to state: ");
            Serial.println(servoState);
        }
    }

    int getCurrentState() const
    {
        return lastServoState;
    }
};

#endif
