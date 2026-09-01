#ifndef EVENTFACTORY_H
#define EVENTFACTORY_H

#include <string>

#include "Event.h"

class EventFactory
{
public:
    static Event createEvent(
        const std::string &id);
};

#endif
