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
        if (!pContext->isUnconnected())
        {
            pContext->setAutomatic();
            if (this->checkAndSetJustEntered())
            {
                conditionStartTime = 0;
                pValveMotor->close();
            }
            if (pButton->isPressed())
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
        }
        else
        {
            setState(UNCONNECTED);
        }

        break;

    case MANUAL:
        if (!pContext->isUnconnected())
        {
            pContext->setManual();
            if (this->checkAndSetJustEntered())
            {
                conditionStartTime = 0;
            }
            if (pButton->isPressed())
            {
                setState(AUTOMATIC);
            }
            int potValue = pContext->getPotValue();
            int angle = map(potValue, 0, 1023, 0, 90);

            pValveMotor->manuallySetAngle(angle);
            valvePercent = pValveMotor->getOpeningPercent();
            pDisplay->showMessage("MANUAL", "Valve: " + String(valvePercent) + "%");
        }
        else
        {
            setState(UNCONNECTED);
        }

        break;

    case UNCONNECTED:
        if (pContext->isUnconnected())
        {
            pContext->setUnconnected();
            if (this->checkAndSetJustEntered())
            {
                conditionStartTime = 0;
            }
            valvePercent = pValveMotor->getOpeningPercent();
            pDisplay->showMessage("UNCONNECTED", "Valve: " + String(valvePercent) + "%");
        }
        else if (pContext->isAutomatic())
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
