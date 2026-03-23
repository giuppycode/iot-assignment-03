#ifndef __CONNECTION_TASK__
#define __CONNECTION_TASK__

#include "kernel/Task.h"
#include "devices/Led.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"

enum ConnectionState
{
    CHECKING,
    RECONNECTING_WIFI,
    RECONNECTING_MQTT
};

class ConnectionTask : public Task
{
public:
    ConnectionTask(PubSubClient *pClient, Led *pGreenLED, Led *pRedLED);
    void tick() override;

private:
    PubSubClient *pClient;
    Led *pGreenLED;
    Led *pRedLED;
    ConnectionState state;
    void setState(ConnectionState newState);
    unsigned long stateTimestamp;
    bool justEntered;
    bool checkAndSetJustEntered();
};

#endif