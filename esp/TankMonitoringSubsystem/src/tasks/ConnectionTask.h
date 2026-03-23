#ifndef __CONNECTION_TASK__
#define __CONNECTION_TASK__

#include "kernel/Task.h"
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
    ConnectionTask(PubSubClient *pClient);
    void tick() override;

private:
    PubSubClient *pClient;
    ConnectionState state;
    void setState(ConnectionState newState);
};

#endif