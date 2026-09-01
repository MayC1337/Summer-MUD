#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <string>

class Player;
class TimeManager;
class EventManager;

class SaveManager
{
private:
    std::string saveFile;

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
};

#endif
