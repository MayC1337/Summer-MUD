#include "Event.h"

#if __has_include("../player/Player.h") && __has_include("../player/Stats.h")
#include "../player/Player.h"
#include "../player/Stats.h"
#define SUMMER_MUD_HAS_PLAYER_MODULE 1
#elif __has_include("../player/Player.h") && __has_include("../player/stats.h")
#include "../player/Player.h"
#include "../player/stats.h"
#define SUMMER_MUD_HAS_PLAYER_MODULE 1
#else
#define SUMMER_MUD_HAS_PLAYER_MODULE 0
#endif

#include <iostream>

Event::Event(
    const std::string &id,
    const std::string &title,
    const std::string &description,
    const std::vector<std::string> &choices)
    : id(id),
      title(title),
      description(description),
      choices(choices)
{
}

void Event::show() const
{
    std::cout << "\n============================\n";

    std::cout << "事件：" << title << "\n";

    std::cout << description << "\n\n";

    for (std::size_t i = 0; i < choices.size(); ++i)
    {
        std::cout
            << i + 1
            << ". "
            << choices[i]
            << "\n";
    }

    std::cout << "============================\n";
}

std::string Event::getId() const
{
    return id;
}

bool Event::canTrigger(
    const Player &player) const
{
    (void)player;
    return true;
}

void Event::applyChoice(Player &player, int choice) const
{
#if SUMMER_MUD_HAS_PLAYER_MODULE
    Stats &stats = player.getStats();

    if (id == "night_study")
    {
        switch (choice)
        {
        case 1:

            stats.modify(
                StatType::Math,
                5);

            stats.modify(
                StatType::Stress,
                3);

            stats.modify(
                StatType::Stamina,
                -8);

            std::cout
                << "你又刷完了一套数学卷。\n"
                << "数学 +5，压力 +5，体力 -8\n";

            break;

        case 2:

            stats.modify(
                StatType::Health,
                3);

            stats.modify(
                StatType::Stress,
                -3);

            std::cout
                << "你决定早点睡觉。\n"
                << "健康 +3，压力 -3\n";

            break;

        case 3:

            stats.modify(
                StatType::Stress,
                -8);

            stats.modify(
                StatType::Stamina,
                -2);

            std::cout
                << "你玩了一会手机。\n"
                << "压力 -8，体力 -2\n";

            break;
        }
    }
#else
    (void)player;
    (void)choice;
    std::cout << "玩家模块尚未接入，事件效果暂未应用。\n";
#endif
}
