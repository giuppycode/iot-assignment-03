#ifndef __SENSOR_TASK__
#define __SENSOR_TASK__

#include <Arduino.h>
#include "kernel/Task.h"
#include "model/Context.h"

#include "devices/ProximitySensor.h"

class SensorTask : public Task
{

public:
  SensorTask(ProximitySensor *pProximitySensor, Context *pContext);
  void tick();

private:
  enum SensorState
  {
    MONITORING
  };

  void setState(SensorState newState);
  long elapsedTimeInState();
  void log(const String &msg);

  bool checkAndSetJustEntered();

  SensorState state;

  long stateTimestamp;
  bool justEntered;
  unsigned long conditionStartTime;

  ProximitySensor *pProximitySensor;
  Context *pContext;

  int currentPos;
  bool toBeStopped;
};

#endif