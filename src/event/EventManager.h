#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "Event.h"

class Player;

// 维护事件库、已触发记录与玩家选择，并在指定节点驱动事件。
class EventManager
{
private:
    std::vector<Event> events;

    std::set<std::string> triggeredEvents;
    std::map<std::string, int> eventChoices;
    std::string pendingEvent; // 存档时尚未完成选择的事件 ID；空串表示无待处理事件。

public:
    void loadEvents();

    void triggerEvent(
        Player &player);
    void triggerEvent(Player &player, int currentDay);
    void triggerStory(Player &player, int currentDay, int period);
    void showMemories() const;
    const std::string& getPendingEvent() const;
    void setPendingEvent(const std::string& id);

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
