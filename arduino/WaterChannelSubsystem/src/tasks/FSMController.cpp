#include "FSMController.h"
#include <Arduino.h>
#include "kernel/Logger.h"

#define L1 0.20
#define L2 0.30

FSMController::FSMController(DisplayLcd *pDisplay, ValveMotor *pValveMotor, Potentiometer *pPot, Button *pButton, Context *pContext)
{
    this->pDisplay = pDisplay;
    this->pValveMotor = pValveMotor;
    this->pPot = pPot;
    this->pButton = pButton;
    this->pContext = pContext;

    this->conditionStartTime = 0;
    setState(AUTOMATIC);
}

void FSMController::tick()
{
    int valvePercent = pValveMotor->getOpeningPercent();

    switch (state)
    {
    case AUTOMATIC:

        if (this->checkAndSetJustEntered())
        {
            Logger.log("FSM Entered AUTOMATIC");
            pContext->setAutomatic();
            pValveMotor->close();
        }
        if (pButton->wasPressed())
        {
            setState(MANUAL);
        }

        float currentDistance = pContext->getCurrentDistance();
        if (currentDistance > L1 && currentDistance < L2)
        {
            pValveMotor->half();
        }
        else if (currentDistance >= L2)
        {
            pValveMotor->open();
        }
        else
        {
            pValveMotor->close();
        }

        valvePercent = pValveMotor->getOpeningPercent();
        pDisplay->showMessage("AUTOMATIC", "Valve: " + String(valvePercent) + "%");

        if (pContext->isUnconnected())
        {
            setState(UNCONNECTED);
        }

        break;

    case MANUAL:

        if (this->checkAndSetJustEntered())
        {
            pContext->setManual();
            Logger.log("FSM Entered MANUAL");
        }
        if (pButton->wasPressed())
        {
            setState(AUTOMATIC);
        }
        int potValue = pContext->getPotValue();
        int angle = map(potValue, 0, 1023, 0, 90);

        pValveMotor->manuallySetAngle(angle);
        valvePercent = pValveMotor->getOpeningPercent();
        pDisplay->showMessage("MANUAL", "Valve: " + String(valvePercent) + "%");

        if (pContext->isUnconnected())
        {
            setState(UNCONNECTED);
        }

        break;

    case UNCONNECTED:

        if (this->checkAndSetJustEntered())
        {
            Logger.log("FSM Entered UNCONNECTED");
            pContext->setUnconnected();
        }
        valvePercent = pValveMotor->getOpeningPercent();
        pDisplay->showMessage("UNCONNECTED", "Valve: " + String(valvePercent) + "%");

        if (pContext->isAutomatic())
        {
            setState(AUTOMATIC);
        }
        else if (pContext->isManual())
        {
            setState(MANUAL);
        }
    }
}

void FSMController::setState(ControllerState newState)
{
    state = newState;
    stateTimestamp = millis();
    justEntered = true;
}

long FSMController::elapsedTimeInState()
{
    return millis() - stateTimestamp;
}

bool FSMController::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
    {
        justEntered = false;
    }
    return bak;
}
