#ifndef __PUBLISH_TASK__
#define __PUBLISH_TASK__

#include "kernel/Task.h"
#include "model/Context.h"
#include <PubSubClient.h>
#include "config.h"

class PublishTask : public Task
{
public:
    PublishTask(PubSubClient *pClient, Context *pContext);
    void tick() override;

private:
    PubSubClient *pClient;
    Context *pContext;
};

#endif