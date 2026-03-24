#ifndef __POTREADER__
#define __POTREADER__

#include <Arduino.h>
#include "kernel/Task.h"
#include "model/Context.h"
#include "devices/DisplayLcd.h"
#include "devices/ValveMotor.h"
#include "devices/Pot.h"

class PotReader : public Task
{

public:
    PotReader(Potentiometer *pPot, Context *pContext);
    void tick();

private:
    enum PotentiometerState
    {
        READING,
        IDLE
    };

    void setState(PotentiometerState newState);
    long elapsedTimeInState();
    void log(const String &msg);

    bool checkAndSetJustEntered();

    PotentiometerState state;

    long stateTimestamp;
    bool justEntered;
    unsigned long conditionStartTime;

    Potentiometer *pPot;
    Context *pContext;
};

#endif