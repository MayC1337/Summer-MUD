#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <map>
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
    std::map<std::string, int> eventChoices;

public:
    void loadEvents();

    void triggerEvent(
        Player &player);
    void triggerEvent(Player &player, int currentDay);

    bool hasTriggered(
        const std::string &eventId) const;

    void markTriggered(
        const std::string &eventId);

    const std::set<std::string> &getTriggeredEvents() const;
    void setTriggeredEvents(const std::set<std::string> &triggered);
    const std::map<std::string, int> &getEventChoices() const;
    void setEventChoices(const std::map<std::string, int> &choices);
    int getEventChoice(const std::string &eventId) const;
};

#endif
