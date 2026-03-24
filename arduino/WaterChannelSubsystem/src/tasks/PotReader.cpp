#include "PotReader.h"
#include <Arduino.h>
#include "kernel/Logger.h"

PotReader::PotReader(Potentiometer *pPot, Context *pContext)
{
    this->pPot = pPot;
    this->pContext = pContext;
    this->conditionStartTime = 0;
    setState(IDLE);
}

void PotReader::tick()
{
    switch (state)
    {
    case IDLE:
        if (pContext->isManual())
        {
            setState(READING);
        }
        break;

    case READING:
        if (pContext->isManual())
        {
            pPot->sync();
            // qui mi implemneto la conversione da pot a int dei gradi motore
            pContext->setPotValue(pPot->getValue()); // Converti il valore, non credo vada bene pero serve float
        }
        else if (pContext->isAutomatic())
        {
            setState(IDLE);
        }
    }
}
void PotReader::setState(PotentiometerState newState)
{
    state = newState;
    stateTimestamp = millis();
    justEntered = true;
}

long PotReader::elapsedTimeInState()
{
    return millis() - stateTimestamp;
}

bool PotReader::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
    {
        justEntered = false;
    }
    return bak;
}
