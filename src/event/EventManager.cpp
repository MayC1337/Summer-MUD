#include "EventManager.h"

#include "EventFactory.h"
#include "StoryData.h"
#include "../core/ConsoleUI.h"
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

void EventManager::triggerStory(Player& player, int currentDay, int period)
{
    for (const auto& node : StoryData::nodes())
    {
        if (node.day > currentDay || node.period != period || hasTriggered(node.id)) continue;
        if (node.previous[0] != '\0' && !hasTriggered(node.previous)) continue;
        if (node.previous[0] != '\0')
            std::cout << (getEventChoice(node.previous) == 1 ? node.warm : node.distant) << '\n';
        const Event event = EventFactory::createEvent(node.id);
        event.show();
        int choice = 0;
        if (!CommandParser::readChoice(choice, 1, 3)) return;
        event.applyChoice(player, choice);
        markTriggered(node.id);
        eventChoices[node.id] = choice;
        return; // 每个时段最多一段故事，旧存档按顺序补上
    }
}

void EventManager::showMemories() const
{
    ConsoleUI::boxTop("那些留在六月的事");
    const char* finals[] = {"desk_4", "teacher_4", "family_4", "cat_4"};
    int shown = 0;
    for (const char* id : finals)
    {
        const int choice = getEventChoice(id);
        if (choice >= 1 && choice <= 3)
        {
            const auto* node = StoryData::find(id);
            ConsoleUI::boxLine(node->title);
            ConsoleUI::boxLine(node->choices[static_cast<std::size_t>(choice - 1)].reply);
            ++shown;
        }
    }
    if (shown == 0) ConsoleUI::boxLine("未写完的故事，也可以留给下一个夏天。");
    ConsoleUI::boxDivider("一封告别留言");
    if (getEventChoice("desk_4") == 1)
        ConsoleUI::boxLine("林晓：以后遇到难题，记得给我写信。");
    else if (getEventChoice("family_4") != 0)
        ConsoleUI::boxLine("家人：记得好好吃饭，家里的灯一直为你亮着。");
    else ConsoleUI::boxLine("老师：铃声之后，仍有很多答案等你亲自去找。");
    ConsoleUI::boxBottom();
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
    if (pendingEvent.empty() && (candidates.empty() || chance(generator) > 65))
    {
        static const char *const quietNarrations[] = {
            "今天没有发生特别的事，日历安静地翻过一页。",
            "教室里的风扇缓慢转动，这只是普通却珍贵的一天。",
            "晚自习铃声响起，一天在笔尖与翻书声中结束。"};
        std::uniform_int_distribution<int> quietChoice(0, 2);
        std::cout << quietNarrations[quietChoice(generator)] << '\n';
        return;
    }

    if (pendingEvent.empty())
    {
        std::uniform_int_distribution<std::size_t> select(0, candidates.size() - 1);
        pendingEvent = candidates[select(generator)]->getId();
    }
    Event event = EventFactory::createEvent(pendingEvent);
    event.show();

    int choice = 0;
    if (!CommandParser::readChoice(choice, 1, 3, "请输入你的选择："))
    {
        return;
    }

    event.applyChoice(player, choice);
    markTriggered(event.getId());
    eventChoices[event.getId()] = choice;
    pendingEvent.clear();
}

const std::string& EventManager::getPendingEvent() const { return pendingEvent; }
void EventManager::setPendingEvent(const std::string& id) { pendingEvent = id; }

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
