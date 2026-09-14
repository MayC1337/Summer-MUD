#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <string>

class Player;
class TimeManager;
class EventManager;
struct GameProgress;

// 负责将各模块的可恢复状态写入同一个存档文件，并在读档时重建它们。
class SaveManager
{
private:
    std::string saveFile; // 存档目标路径，由游戏管理器在构造时指定。

public:
    explicit SaveManager(const std::string &fileName = "save.txt");

    bool saveGame(
        const Player &player,
        const TimeManager &timeManager,
        const EventManager &eventManager);

    bool loadGame(
        Player &player,
        TimeManager &timeManager,
        EventManager &eventManager);

    bool hasSave() const;
    bool saveGame(const Player& player, const TimeManager& timeManager,
        const EventManager& eventManager, const GameProgress& progress);
    bool loadGame(Player& player, TimeManager& timeManager,
        EventManager& eventManager, GameProgress& progress);
};

#endif
