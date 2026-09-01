#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <set>
#include <string>
#include <vector>

#include "Event.h"

class Player;

class EventManager
{
private:
    std::vector<Event> events;

    std::set<std::string> triggeredEvents;

public:
    void loadEvents();

    void triggerEvent(
        Player &player);

    bool hasTriggered(
        const std::string &eventId) const;

    void markTriggered(
        const std::string &eventId);
};

#endif
