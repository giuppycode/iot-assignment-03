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
    switch (state)
    {
    case AUTOMATIC:
        if (!pContext->isUnconnected())
        {
            pContext->setAutomatic();
            if (this->checkAndSetJustEntered())
            {
                conditionStartTime = 0;
                pDisplay->showMessage("AUTOMATIC");
                pValveMotor->close();
            }
            if (pButton->isPressed())
            {
                setState(MANUAL);
            }

            float currentDistance = pContext->getCurrentDistance();
            if (currentDistance > L1 && currentDistance < L2)
            {
                pValveMotor->half(); // al 50%
            }
            else if (currentDistance >= L2)
            {
                pValveMotor->open(); // al 100%
            }
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
                pDisplay->showMessage("MANUAL");
            }
            if (pButton->isPressed())
            {
                setState(AUTOMATIC);
            }
            int potValue = pContext->getPotValue();
            int angle = (int)(potValue * 180); // Converti il valore del

            pValveMotor->manuallySetAngle(angle);
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
                pDisplay->showMessage("UNCONNECTED");
            }
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
