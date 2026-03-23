#include "TankMonitoringPlatform.h"
#include "kernel/Logger.h"

TankMonitoringPlatform::TankMonitoringPlatform()
{
  pSonar = new Sonar(SONAR_ECHO_PIN, SONAR_TRIG_PIN, 30000);
  pMQTTClient = new PubSubClient(espClient);
}

void TankMonitoringPlatform::init()
{
  pMQTTClient->setServer(MQTT_BROKER, MQTT_PORT);
}

Sonar *TankMonitoringPlatform::getSonar()
{
  return pSonar;
}

PubSubClient *TankMonitoringPlatform::getMQTTClient()
{
  return pMQTTClient;
}