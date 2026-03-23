#include "Context.h"

Context::Context()
{
  currentDistance = 0.0;
}

void Context::setCurrentDistance(float dist)
{
  currentDistance = dist;
}

float Context::getCurrentDistance()
{
  return currentDistance;
}

