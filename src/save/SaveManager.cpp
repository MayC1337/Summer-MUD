#include "SaveManager.h"

#include "../core/TimeManager.h"
#include "../core/GameProgress.h"
#include "../world/CampusMap.h"
#include "../event/EventFactory.h"
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
#include <filesystem>
#include <iomanip>
#include <chrono>
#include <cstdio>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace
{
const char *const SAVE_VERSION = "SummerMUDSaveV4";
const char *const V3_SAVE_VERSION = "SummerMUDSaveV3";
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

bool validProgress(const GameProgress& progress, int elapsedDays, int totalDays)
{
    const int stage = static_cast<int>(progress.stage);
    if (stage < 0 || stage > 7 || progress.period < 0 || progress.period > 3 ||
        progress.dailyGoal < 1 || progress.dailyGoal > 3 || progress.goalSubject < 5 ||
        progress.goalSubject > 8 || !isValidStat(progress.goalStart) ||
        progress.lastWeeklyScore < -1 || progress.lastWeeklyScore > 750 ||
        !CampusMap().getRooms().count(progress.location) ||
        !CampusMap().getRooms().count(progress.previousEveningLocation) || progress.npcConversations.size() > 1000)
        return false;
    if (elapsedDays == totalDays && stage != 0) return false;
    const bool studyDay = elapsedDays % 7 < 5;
    if ((stage >= 1 && stage <= 4 && !studyDay) || (stage == 5 && studyDay)) return false;
    if (stage != 3 && !progress.pendingChoices.empty()) return false;
    if (!progress.pendingEvent.empty())
    {
        if (progress.stage != GameProgress::Stage::DailyEvent) return false;
        try { EventFactory::createEvent(progress.pendingEvent); } catch (...) { return false; }
    }
    const auto validChoices = [](const std::vector<int>& choices)
    {
        if (choices.size() > 32) return false;
        for (int value : choices) if (value < 0 || value > 7) return false;
        return true;
    };
    if (!validChoices(progress.pendingChoices)) return false;
    for (const auto& choices : progress.previousActions) if (!validChoices(choices)) return false;
    for (const auto& key : progress.npcConversations)
        if (key.empty() || key.size() > 64 || key.find_first_of("\r\n") != std::string::npos) return false;
    return true;
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
    return saveGame(player, timeManager, eventManager, GameProgress{});
}

bool SaveManager::saveGame(const Player& player, const TimeManager& timeManager,
    const EventManager& eventManager, const GameProgress& progress)
{
    if (!validProgress(progress, timeManager.getElapsedDays(), timeManager.getTotalDays()) ||
        player.getName().empty() || player.getName().find_first_of("\r\n") != std::string::npos) return false;
    const auto destination = std::filesystem::u8path(saveFile);
    auto temporary = destination;
    temporary += ".tmp." + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    struct TemporaryFile
    {
        std::filesystem::path path;
        ~TemporaryFile() { std::error_code error; std::filesystem::remove(path, error); }
    } cleanup{temporary};
    std::ofstream output(temporary, std::ios::out | std::ios::trunc);
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

    output << static_cast<int>(progress.stage) << ' ' << progress.period << ' '
           << progress.dailyGoal << ' ' << progress.goalSubject << ' ' << progress.goalStart
           << ' ' << progress.lastWeeklyScore << ' ' << progress.repeat << '\n';
    output << std::quoted(progress.location) << ' ' << std::quoted(progress.pendingEvent)
           << ' ' << std::quoted(progress.previousEveningLocation) << '\n';
    const auto writeChoices = [&output](const std::vector<int>& choices)
    {
        output << choices.size();
        for (int choice : choices) output << ' ' << choice;
        output << '\n';
    };
    for (const auto& choices : progress.previousActions) writeChoices(choices);
    writeChoices(progress.pendingChoices);
    output << progress.npcConversations.size() << '\n';
    for (const auto& key : progress.npcConversations) output << std::quoted(key) << '\n';
    output.flush();
    if (!output) return false;
    output.close();
    if (!output) return false;
#ifdef _WIN32
    return MoveFileExW(temporary.c_str(), destination.c_str(),
        MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    return std::rename(temporary.c_str(), destination.c_str()) == 0;
#endif
}

bool SaveManager::loadGame(
    Player &player,
    TimeManager &timeManager,
    EventManager &eventManager)
{
    GameProgress unused;
    return loadGame(player, timeManager, eventManager, unused);
}

bool SaveManager::loadGame(Player& player, TimeManager& timeManager,
    EventManager& eventManager, GameProgress& progress)
{
    std::ifstream input(std::filesystem::u8path(saveFile));
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
        (version != SAVE_VERSION && version != V3_SAVE_VERSION && version != PREVIOUS_SAVE_VERSION &&
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
    if (version == SAVE_VERSION || version == V3_SAVE_VERSION || version == PREVIOUS_SAVE_VERSION)
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
    if (version == SAVE_VERSION || version == V3_SAVE_VERSION)
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

    GameProgress restoredProgress;
    if (version == SAVE_VERSION)
    {
        std::string line;
        if (!std::getline(input, line)) return false;
        std::istringstream parser(line);
        int stage = 0;
        int repeat = 0;
        std::string extra;
        if (!(parser >> stage >> restoredProgress.period >> restoredProgress.dailyGoal >>
              restoredProgress.goalSubject >> restoredProgress.goalStart >>
              restoredProgress.lastWeeklyScore >> repeat) || (parser >> extra) ||
              repeat < 0 || repeat > 1) return false;
        restoredProgress.stage = static_cast<GameProgress::Stage>(stage);
        restoredProgress.repeat = repeat == 1;
        if (!std::getline(input, line)) return false;
        std::istringstream locationParser(line);
        if (!(locationParser >> std::quoted(restoredProgress.location) >>
              std::quoted(restoredProgress.pendingEvent) >>
              std::quoted(restoredProgress.previousEveningLocation)) || (locationParser >> extra)) return false;
        const auto readChoices = [&input](std::vector<int>& choices)
        {
            std::string row;
            if (!std::getline(input, row)) return false;
            std::istringstream numbers(row);
            int count = 0;
            if (!(numbers >> count) || count < 0 || count > 32) return false;
            for (int i = 0; i < count; ++i)
            {
                int choice = 0;
                if (!(numbers >> choice)) return false;
                choices.push_back(choice);
            }
            std::string extraValue;
            return !(numbers >> extraValue);
        };
        for (auto& choices : restoredProgress.previousActions) if (!readChoices(choices)) return false;
        if (!readChoices(restoredProgress.pendingChoices)) return false;
        int count = 0;
        if (!readIntLine(input, count) || count < 0 || count > 1000) return false;
        for (int i = 0; i < count; ++i)
        {
            if (!std::getline(input, line)) return false;
            std::istringstream keyParser(line);
            std::string key;
            if (!(keyParser >> std::quoted(key)) || (keyParser >> extra) ||
                !restoredProgress.npcConversations.insert(key).second) return false;
        }
    }
    if (!validProgress(restoredProgress, elapsedDays, timeManager.getTotalDays())) return false;
    if (!restoredProgress.pendingEvent.empty() && triggered.count(restoredProgress.pendingEvent)) return false;

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
        eventManager.setPendingEvent(restoredProgress.pendingEvent);
        progress = restoredProgress;

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
    std::ifstream input(std::filesystem::u8path(saveFile));
    return static_cast<bool>(input);
}
