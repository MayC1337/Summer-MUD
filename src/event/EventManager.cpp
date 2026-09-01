#include "EventManager.h"

#include "EventFactory.h"

#include <iostream>

void EventManager::loadEvents()
{
    events.clear();

    events.push_back(
        EventFactory::createEvent(
            "night_study"));

    events.push_back(
        EventFactory::createEvent(
            "classmate_help"));

    events.push_back(
        EventFactory::createEvent(
            "teacher_talk"));
}

bool EventManager::hasTriggered(
    const std::string &eventId) const
{
    return triggeredEvents.find(eventId) != triggeredEvents.end();
}

void EventManager::markTriggered(
    const std::string &eventId)
{
    triggeredEvents.insert(eventId);
}

void EventManager::triggerEvent(
    Player &player)
{
    for (Event &event : events)
    {
        if (
            !hasTriggered(event.getId()) &&
            event.canTrigger(player))
        {
            event.show();

            int choice = 0;

            std::cout
                << "请输入你的选择：";

            std::cin >> choice;

            event.applyChoice(
                player,
                choice);

            markTriggered(
                event.getId());

            return;
        }
    }
    std::cout
        << "今天没有发生特殊事件。\n";
}

const std::set<std::string> &EventManager::getTriggeredEvents() const
{
    return triggeredEvents;
}

void EventManager::setTriggeredEvents(const std::set<std::string> &events)
{
    triggeredEvents = events;
}
