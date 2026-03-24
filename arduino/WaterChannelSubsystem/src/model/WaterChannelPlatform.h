#ifndef __WATER_CHANNEL_PLATFORM__
#define __WATER_CHANNEL_PLATFORM__

#include "config.h"
#include "devices/Button.h"
#include "devices/ValveMotor.h"
#include "devices/DisplayLcd.h"
#include "devices/Pot.h"

class WaterChannelPlatform
{

public:
  WaterChannelPlatform();
  void init();

  Button *getButton();
  ValveMotor *getValveMotor();
  DisplayLcd *getDisplayLcd();
  Potentiometer *getPot();

private:
  Button *pButton;
  ValveMotor *pValveMotor;
  DisplayLcd *pDisplayLcd;
  Potentiometer *pPot;
};

#endif