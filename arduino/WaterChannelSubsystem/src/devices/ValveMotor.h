#ifndef __VALVE_MOTOR__
#define __VALVE_MOTOR__

#include <Arduino.h>
#include "ServoTimer2.h"

class ValveMotor
{
public:
    ValveMotor(int pin);

    void open();
    void close();
    void half();
    void manuallySetAngle(int angle);
    int getOpeningPercent();
    int getCurrentAngle();

private:
    int _pin;
    ServoTimer2 motor;
    int currentAngle;
    int currentPercent;
    void setAngle(int angle);
    bool isAttached();
    void safeAttach();
    void safeDetach();
};

#endif