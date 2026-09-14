#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <vector>

class Player;

// 一个可触发的传统事件：负责显示、资格判断和选项效果。
class Event
{
private:
    std::string id;

    std::string title;

    std::string description;

    std::vector<std::string> choices; // 面向玩家展示的选项文本。

    Event(
        const std::string &eventId,
        const std::string &eventTitle,
        const std::string &eventDescription,
        const std::vector<std::string> &eventChoices);

    friend class EventFactory;

public:
    void show() const;

    void applyChoice(Player &player, int choice) const;

    bool canTrigger(const Player &player) const;

    std::string getId() const;
};

#endif
