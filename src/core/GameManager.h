#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "TimeManager.h"
#include "../event/EventManager.h"

#include <string>

class Player;

class GameManager
{
private:
    bool running;
    std::string playerName;
    Player *player;
    TimeManager timeManager;
    EventManager eventManager;

    GameManager();

    void showWelcome() const;
    void createPlayer();
    void processCurrentDay();
    void executeDailyAction();
    void calculateExam();
    void takeWeeklyRest();
    void showDayHeader() const;
    void triggerDailyEvent();

public:
    static GameManager &getInstance();

    void startGame();
    void run();
    void endGame();

    GameManager(const GameManager &) = delete;
    GameManager &operator=(const GameManager &) = delete;
};

#endif
