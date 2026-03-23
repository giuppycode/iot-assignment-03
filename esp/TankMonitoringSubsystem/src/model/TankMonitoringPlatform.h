#ifndef __TANK_MONITORING_PLATFORM__
#define __TANK_MONITORING_PLATFORM__

#include "config.h"
#include "devices/Sonar.h"
#include "devices/Led.h"
#include <WiFi.h>
#include <PubSubClient.h>

class TankMonitoringPlatform
{
public:
  TankMonitoringPlatform();
  void init();
  Sonar *getSonar();
  Led *getGreenLED();
  Led *getRedLED();
  PubSubClient *getMQTTClient();

private:
  Sonar *pSonar;
  Led *pGreenLED;
  Led *pRedLED;
  WiFiClient espClient;
  PubSubClient *pMQTTClient;
};

#endif