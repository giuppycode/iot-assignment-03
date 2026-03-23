#ifndef __TANK_MONITORING_PLATFORM__
#define __TANK_MONITORING_PLATFORM__

#include "config.h"
#include "devices/Sonar.h"
#include <WiFi.h>
#include <PubSubClient.h>

class TankMonitoringPlatform
{
public:
  TankMonitoringPlatform();
  void init();
  Sonar *getSonar();
  PubSubClient *getMQTTClient();

private:
  Sonar *pSonar;
  WiFiClient espClient;
  PubSubClient *pMQTTClient;
};

#endif