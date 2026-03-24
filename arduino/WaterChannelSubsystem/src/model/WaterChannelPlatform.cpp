#include "WaterChannelPlatform.h"
#include <Arduino.h>

#include "kernel/MsgService.h"
#include "kernel/Logger.h"
#include "config.h"

#include "devices/DisplayLcd.h"
#include "devices/ValveMotor.h"
#include "devices/ButtonImpl.h"
#include "devices/Pot.h"

void wakeUp() {}

WaterChannelPlatform::WaterChannelPlatform()
{
  pButton = new ButtonImpl(BT_PIN);
  pDisplayLcd = new DisplayLcd(LCD_I2C_ADDRESS, LCD_COLS, LCD_ROWS);
  pValveMotor = new ValveMotor(MOTOR_PIN);
  pPot = new Potentiometer(POT_PIN);
}

void WaterChannelPlatform::init()
{
  pDisplayLcd->init();
  pValveMotor->close();
  Logger.log("WaterChannelPlatform initialized");
}

Button *WaterChannelPlatform::getButton()
{
  return this->pButton;
}

ValveMotor *WaterChannelPlatform::getValveMotor()
{
  return this->pValveMotor;
}

DisplayLcd *WaterChannelPlatform::getDisplayLcd()
{
  return this->pDisplayLcd;
}

Potentiometer *WaterChannelPlatform::getPot()
{
  return this->pPot;
}