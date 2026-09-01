#include "SaveManager.h"

#include "../core/TimeManager.h"
#include "../event/EventManager.h"
#include "../player/Player.h"
#include "../player/Stats.h"

#include <fstream>
#include <set>
#include <sstream>
#include <string>

namespace
{
const char *const SAVE_VERSION = "SummerMUDSaveV1";
const std::size_t MAX_SAVED_EVENTS = 10000;

bool readIntLine(std::istream &input, int &value)
{
    std::string line;
    if (!std::getline(input, line))
    {
        return false;
    }

    std::istringstream parser(line);
    if (!(parser >> value))
    {
        return false;
    }

    std::string extra;
    return !(parser >> extra);
}

bool isValidStat(int value)
{
    return value >= 0 && value <= 100;
}
}

SaveManager::SaveManager(const std::string &fileName)
    : saveFile(fileName)
{
}

bool SaveManager::saveGame(
    const Player &player,
    const TimeManager &timeManager,
    const EventManager &eventManager)
{
    std::ofstream output(saveFile.c_str(), std::ios::out | std::ios::trunc);
    if (!output)
    {
        return false;
    }

    const Stats &stats = player.getStats();
    const std::set<std::string> &triggered = eventManager.getTriggeredEvents();

    output << SAVE_VERSION << '\n';
    output << player.getName() << '\n';
    output << player.getMoney() << '\n';
    output << stats.get(StatType::Intelligence) << ' '
           << stats.get(StatType::EQ) << ' '
           << stats.get(StatType::Stamina) << ' '
           << stats.get(StatType::Health) << ' '
           << stats.get(StatType::Stress) << ' '
           << stats.get(StatType::Chinese) << ' '
           << stats.get(StatType::Math) << ' '
           << stats.get(StatType::English) << ' '
           << stats.get(StatType::Science) << '\n';
    output << timeManager.getElapsedDays() << '\n';
    output << triggered.size() << '\n';

    for (const std::string &eventId : triggered)
    {
        output << eventId << '\n';
    }

    output.flush();
    return static_cast<bool>(output);
}

bool SaveManager::loadGame(
    Player &player,
    TimeManager &timeManager,
    EventManager &eventManager)
{
    std::ifstream input(saveFile.c_str());
    if (!input)
    {
        return false;
    }

    std::string version;
    std::string name;
    int money = 0;
    int values[9] = {};
    int elapsedDays = 0;
    int eventCount = 0;

    if (!std::getline(input, version) || version != SAVE_VERSION ||
        !std::getline(input, name) || name.empty() ||
        !readIntLine(input, money) || money < 0)
    {
        return false;
    }

    std::string statsLine;
    if (!std::getline(input, statsLine))
    {
        return false;
    }

    std::istringstream statsParser(statsLine);
    for (int &value : values)
    {
        if (!(statsParser >> value) || !isValidStat(value))
        {
            return false;
        }
    }
    std::string extraStat;
    if ((statsParser >> extraStat) ||
        !readIntLine(input, elapsedDays) ||
        elapsedDays < 0 || elapsedDays > timeManager.getTotalDays() ||
        !readIntLine(input, eventCount) || eventCount < 0 ||
        static_cast<std::size_t>(eventCount) > MAX_SAVED_EVENTS)
    {
        return false;
    }

    std::set<std::string> triggered;
    for (int i = 0; i < eventCount; ++i)
    {
        std::string eventId;
        if (!std::getline(input, eventId) || eventId.empty() ||
            !triggered.insert(eventId).second)
        {
            return false;
        }
    }

    std::string trailing;
    while (std::getline(input, trailing))
    {
        if (!trailing.empty())
        {
            return false;
        }
    }

    try
    {
        player.setName(name);
        player.changeMoney(money - player.getMoney());

        Stats &stats = player.getStats();
        const StatType types[9] = {
            StatType::Intelligence, StatType::EQ, StatType::Stamina,
            StatType::Health, StatType::Stress, StatType::Chinese,
            StatType::Math, StatType::English, StatType::Science};
        for (int i = 0; i < 9; ++i)
        {
            stats.set(types[i], values[i]);
        }

        timeManager.setElapsedDays(elapsedDays);
        eventManager.setTriggeredEvents(triggered);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool SaveManager::hasSave() const
{
    std::ifstream input(saveFile.c_str());
    return static_cast<bool>(input);
}
