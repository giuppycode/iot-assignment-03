#include "SensorTask.h"
#include <Arduino.h>
#include "kernel/Logger.h"

SensorTask::SensorTask(ProximitySensor *pProximitySensor, Context *pContext)
{
    this->pProximitySensor = pProximitySensor;
    this->pContext = pContext;

    this->conditionStartTime = 0;
    setState(MONITORING);
}

void SensorTask::tick()
{
    switch (state)
    {
    case MONITORING:
        if (checkAndSetJustEntered())
        {
            Logger.log("[SensorTask] Entering MONITORING");
        }

        float distance = pProximitySensor->getDistance();
        pContext->setCurrentDistance(distance);
        // Logger.log("[SensorTask] Distance: " + String(distance, 1) + " cm");

        break;
    }
}

void SensorTask::setState(SensorState newState)
{
    state = newState;
    stateTimestamp = millis();
    justEntered = true;
}

bool SensorTask::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
    {
        justEntered = false;
    }
    return bak;
}
