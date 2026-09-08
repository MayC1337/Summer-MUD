#include "EventManager.h"

#include "EventFactory.h"
#include "../command/CommandParser.h"

#include <iostream>
#include <random>

namespace
{
bool isAvailableOnDay(const std::string &id, int day)
{
    if (id == "mock_exam_slump") return day >= 15;
    if (id == "graduation_photo") return day >= 22;
    if (id == "final_night") return day >= 29;
    if (id == "parent_argument") return day >= 8;
    return true;
}
}

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

    const char *const moreEventIds[] = {
        "rainy_day", "surprise_quiz", "lost_notebook", "family_snack",
        "insomnia", "sports_injury", "old_friend_message",
        "study_breakthrough", "parent_argument", "graduation_photo",
        "mock_exam_slump", "final_night"};

    for (const char *eventId : moreEventIds)
    {
        events.push_back(EventFactory::createEvent(eventId));
    }
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
    triggerEvent(player, 1);
}

void EventManager::triggerEvent(Player &player, int currentDay)
{
    std::vector<Event *> candidates;
    for (Event &event : events)
    {
        if (!hasTriggered(event.getId()) && event.canTrigger(player) &&
            isAvailableOnDay(event.getId(), currentDay))
        {
            candidates.push_back(&event);
        }
    }

    static std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> chance(1, 100);
    if (candidates.empty() || chance(generator) > 65)
    {
        static const char *const quietNarrations[] = {
            "今天没有发生特别的事，日历安静地翻过一页。",
            "教室里的风扇缓慢转动，这只是普通却珍贵的一天。",
            "晚自习铃声响起，一天在笔尖与翻书声中结束。"};
        std::uniform_int_distribution<int> quietChoice(0, 2);
        std::cout << quietNarrations[quietChoice(generator)] << '\n';
        return;
    }

    std::uniform_int_distribution<std::size_t> select(0, candidates.size() - 1);
    Event &event = *candidates[select(generator)];
    event.show();

    int choice = 0;
    if (!CommandParser::readChoice(choice, 1, 3, "请输入你的选择："))
    {
        return;
    }

    event.applyChoice(player, choice);
    markTriggered(event.getId());
    eventChoices[event.getId()] = choice;
}

const std::set<std::string> &EventManager::getTriggeredEvents() const
{
    return triggeredEvents;
}

void EventManager::setTriggeredEvents(const std::set<std::string> &triggered)
{
    triggeredEvents = triggered;
}

const std::map<std::string, int> &EventManager::getEventChoices() const
{
    return eventChoices;
}

void EventManager::setEventChoices(const std::map<std::string, int> &choices)
{
    eventChoices = choices;
}

int EventManager::getEventChoice(const std::string &eventId) const
{
    const auto it = eventChoices.find(eventId);
    return it == eventChoices.end() ? 0 : it->second;
}
