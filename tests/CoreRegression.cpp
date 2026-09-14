#include "../src/command/CommandParser.h"
#include "../src/core/GameProgress.h"
#include "../src/core/TimeManager.h"
#include "../src/event/EventManager.h"
#include "../src/npc/NPC.h"
#include "../src/player/Player.h"
#include "../src/save/SaveManager.h"
#include "../src/world/CampusMap.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

std::string readFile(const std::string& path)
{
    std::ifstream input(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

int main()
{
    std::ostringstream output;
    auto* oldOutput = std::cout.rdbuf(output.rdbuf());
    std::istringstream input("1abc\n1.5\n1 2\n9999999999999999999999\n\n2\n");
    auto* oldInput = std::cin.rdbuf(input.rdbuf());
    int choice = 0;
    assert(CommandParser::readChoice(choice, 1, 3) && choice == 2);
    assert(!CommandParser::readChoice(choice, 1, 3));
    CommandParser::setCommandHandler([](const std::string&) { return false; });
    bool interrupted = false;
    try { CommandParser::readChoice(choice, 1, 3); }
    catch (const InputInterrupted& error) { interrupted = error.endOfInput; }
    assert(interrupted);
    CommandParser::setCommandHandler({});
    std::cin.clear();

    CampusMap map;
    int arrivals = 0;
    map.setArrivalHandler([&arrivals]() { ++arrivals; });
    assert(map.getRooms().size() == 9);
    for (const auto& entry : map.getRooms())
    {
        assert(!map.routeTo(entry.first).empty());
        for (const auto& exit : entry.second.exits) assert(map.getRooms().count(exit.second));
    }
    assert(map.move("north") && map.currentRoom().id == "classroom");
    assert(!map.move("north") && map.currentRoom().id == "classroom");
    assert(!map.setLocation("nowhere"));
    assert(map.travelTo("shop") && map.currentRoom().id == "shop");
    assert(!map.travelTo("nowhere"));
    assert(arrivals == 2); // 成功移动和直达各一次，失败操作不触发人物提示。

    Player player("校园测试");
    TimeManager time;
    time.setElapsedDays(2);
    EventManager events;
    events.loadEvents();
    events.markTriggered("desk_1");
    events.setEventChoices({{"desk_1", 1}});
    GameProgress progress;
    progress.stage = GameProgress::Stage::Action;
    progress.period = 3;
    progress.location = "library";
    progress.previousEveningLocation = "shop";
    progress.lastWeeklyScore = 615;
    progress.previousActions[0] = {1};
    progress.pendingChoices = {6, 1, 2};
    progress.npcConversations.insert("linxiao@3");
    SaveManager save("regression-save.txt");
    assert(save.saveGame(player, time, events, progress));
    const auto original = readFile("regression-save.txt");
    Player restored("not restored");
    TimeManager restoredTime;
    EventManager restoredEvents;
    GameProgress restoredProgress;
    assert(save.loadGame(restored, restoredTime, restoredEvents, restoredProgress));
    assert(restoredTime.getElapsedDays() == 2 && restored.getName() == player.getName());
    assert(restoredProgress.location == "library" && restoredProgress.period == 3);
    assert(restoredProgress.previousEveningLocation == "shop");
    assert(restoredProgress.stage == GameProgress::Stage::Action);
    assert(restoredProgress.pendingChoices == progress.pendingChoices);
    assert(restoredProgress.previousActions == progress.previousActions);
    assert(restoredProgress.npcConversations == progress.npcConversations);
    assert(restoredProgress.lastWeeklyScore == 615);
    assert(restoredEvents.getEventChoices() == events.getEventChoices());
    progress.location = "broken-room";
    assert(!save.saveGame(player, time, events, progress));
    assert(readFile("regression-save.txt") == original);
    progress.location = "library";

#ifdef _WIN32
    const HANDLE locked = CreateFileW(L"regression-save.txt", GENERIC_READ, FILE_SHARE_READ,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    assert(locked != INVALID_HANDLE_VALUE);
    assert(!save.saveGame(player, time, events, progress));
    assert(readFile("regression-save.txt") == original);
    CloseHandle(locked);
#endif
    // 新写入成功后，失败尝试遗留的临时文件也必须消失。
    assert(save.saveGame(player, time, events, progress));
    for (const auto& entry : std::filesystem::directory_iterator("."))
        assert(entry.path().filename().string().find("regression-save.txt.tmp.") == std::string::npos);

    { std::ofstream bad("bad-save.txt"); bad << original << "unexpected trailing data\n"; }
    SaveManager bad("bad-save.txt");
    restored.getStats().set(StatType::Math, 17);
    assert(!bad.loadGame(restored, restoredTime, restoredEvents, restoredProgress));
    assert(restored.getStats().get(StatType::Math) == 17);
    assert(restoredProgress.location == "library");

    for (int version = 1; version <= 3; ++version)
    {
        std::ofstream legacy("legacy-save.txt");
        legacy << "SummerMUDSaveV" << version << "\n旧玩家\n123\n40 40 80 80 10 45 45 45 45\n11\n0\n";
        if (version >= 2) legacy << "0\n";
        if (version >= 3) legacy << "0\n";
        legacy.close();
        SaveManager legacySave("legacy-save.txt");
        assert(legacySave.loadGame(restored, restoredTime, restoredEvents, restoredProgress));
        assert(restoredTime.getElapsedDays() == 11 && restored.getMoney() == 123);
        assert(restoredProgress.stage == GameProgress::Stage::DayStart);
        assert(restoredProgress.pendingChoices.empty());
    }
    const auto npcs = NPC::createCampusNPCs();
    assert(npcs.front().isPresent("canteen", 1));
    assert(!npcs.front().isPresent("canteen", 3));
    player.getStats().set(StatType::Stress, 20);
    npcs.front().talk(player, events, true);
    assert(player.getStats().get(StatType::Stress) == 19);
    npcs.front().talk(player, events, false);
    assert(player.getStats().get(StatType::Stress) == 19);
    std::cin.rdbuf(oldInput);
    std::cout.rdbuf(oldOutput);
    std::cout << "Input, map, NPC, V1-V4 compatibility and atomic-save checks passed.\n";
}
