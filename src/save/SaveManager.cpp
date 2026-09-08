#include "SaveManager.h"

#include "../core/TimeManager.h"
#include "../event/EventManager.h"
#include "../player/Inventory.h"
#include "../player/Item.h"
#include "../player/Player.h"
#include "../player/Stats.h"

#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace
{
const char *const SAVE_VERSION = "SummerMUDSaveV3";
const char *const PREVIOUS_SAVE_VERSION = "SummerMUDSaveV2";
const char *const LEGACY_SAVE_VERSION = "SummerMUDSaveV1";
const std::size_t MAX_SAVED_EVENTS = 10000;
const std::size_t MAX_SAVED_ITEMS = 1000;

std::unique_ptr<Item> createSavedItem(const std::string &id)
{
    if (id == "snack") return std::make_unique<Item>(id, "零食", 20);
    if (id == "novel") return std::make_unique<Item>(id, "小说", 80);
    if (id == "mp4") return std::make_unique<Item>(id, "MP4", 300);
    if (id == "phone") return std::make_unique<Item>(id, "手机", 800);
    return nullptr;
}

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

    const auto &items = player.getInventory().getItems();
    output << items.size() << '\n';
    for (const auto &item : items)
    {
        output << item->getId() << '\n';
    }

    const auto &eventChoices = eventManager.getEventChoices();
    output << eventChoices.size() << '\n';
    for (const auto &entry : eventChoices)
    {
        output << entry.first << ' ' << entry.second << '\n';
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

    if (!std::getline(input, version) ||
        (version != SAVE_VERSION && version != PREVIOUS_SAVE_VERSION &&
         version != LEGACY_SAVE_VERSION) ||
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

    std::vector<std::string> itemIds;
    if (version == SAVE_VERSION || version == PREVIOUS_SAVE_VERSION)
    {
        int itemCount = 0;
        if (!readIntLine(input, itemCount) || itemCount < 0 ||
            static_cast<std::size_t>(itemCount) > MAX_SAVED_ITEMS)
        {
            return false;
        }

        for (int i = 0; i < itemCount; ++i)
        {
            std::string itemId;
            if (!std::getline(input, itemId) || !createSavedItem(itemId))
            {
                return false;
            }
            itemIds.push_back(itemId);
        }
    }


    std::map<std::string, int> savedChoices;
    if (version == SAVE_VERSION)
    {
        int choiceCount = 0;
        if (!readIntLine(input, choiceCount) || choiceCount < 0 ||
            static_cast<std::size_t>(choiceCount) > MAX_SAVED_EVENTS)
        {
            return false;
        }

        for (int i = 0; i < choiceCount; ++i)
        {
            std::string line;
            if (!std::getline(input, line))
            {
                return false;
            }

            std::istringstream parser(line);
            std::string eventId;
            int choice = 0;
            std::string extra;
            if (!(parser >> eventId >> choice) || (parser >> extra) ||
                choice < 1 || choice > 3 || !triggered.count(eventId) ||
                !savedChoices.emplace(eventId, choice).second)
            {
                return false;
            }
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
        eventManager.setEventChoices(savedChoices);

        Inventory &inventory = player.getInventory();
        inventory.clear();
        for (const std::string &itemId : itemIds)
        {
            inventory.addItem(createSavedItem(itemId));
        }
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
