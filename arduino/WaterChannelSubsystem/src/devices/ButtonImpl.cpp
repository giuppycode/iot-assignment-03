#include "ButtonImpl.h"

#define DEBOUNCE_MS 50

ButtonImpl::ButtonImpl(int pin)
{
  this->pin = pin;
  pinMode(pin, INPUT);
  lastState = false;
  lastReleaseTime = 0;
}

bool ButtonImpl::isPressed()
{
  return digitalRead(pin) == HIGH;
}

bool ButtonImpl::wasPressed()
{
  bool currentState = digitalRead(pin) == HIGH;
  unsigned long now = millis();

  if (currentState && !lastState && (now - lastReleaseTime > DEBOUNCE_MS))
  {
    lastState = currentState;
    return true;
  }

  lastState = currentState;
  if (!currentState)
  {
    lastReleaseTime = now;
  }
  return false;
}
