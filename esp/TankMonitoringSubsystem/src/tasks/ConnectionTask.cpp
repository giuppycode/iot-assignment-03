#include "ConnectionTask.h"
#include "kernel/Logger.h"

ConnectionTask::ConnectionTask(PubSubClient *pClient)
{
    this->pClient = pClient;
    setState(CHECKING);
}

void ConnectionTask::setState(ConnectionState newState)
{
    state = newState;
}

void ConnectionTask::tick()
{
    // FONDAMENTALE: deve essere chiamato ad ogni tick
    pClient->loop();

    switch (state)
    {
    case CHECKING:
        if (WiFi.status() != WL_CONNECTED)
        {
            Logger.log("[ConnectionTask] WiFi lost, reconnecting...");
            setState(RECONNECTING_WIFI);
        }
        else if (!pClient->connected())
        {
            Logger.log("[ConnectionTask] MQTT lost, reconnecting...");
            setState(RECONNECTING_MQTT);
        }
        break;

    case RECONNECTING_WIFI:
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        if (WiFi.status() == WL_CONNECTED)
        {
            Logger.log("[ConnectionTask] WiFi restored");
            setState(CHECKING);
        }
        break;

    case RECONNECTING_MQTT:
        String clientId = String("esiot-client-") + String(random(0xffff), HEX);
        if (pClient->connect(clientId.c_str()))
        {
            pClient->subscribe(MQTT_TOPIC);
            Logger.log("[ConnectionTask] MQTT restored");
            setState(CHECKING);
        }
        else
        {
            Logger.log("[ConnectionTask] MQTT failed, rc=" + String(pClient->state()));
        }
        break;
    }
}