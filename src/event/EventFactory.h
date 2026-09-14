#ifndef EVENTFACTORY_H
#define EVENTFACTORY_H

#include <string>

#include "Event.h"

// 按事件 ID 构造预定义事件，集中隔离事件配置与调用方。
class EventFactory
{
public:
    static Event createEvent(
        const std::string &id);
};

#endif
