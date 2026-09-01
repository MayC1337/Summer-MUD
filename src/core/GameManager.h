#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "TimeManager.h"

#include <string>

class GameManager
{
private:
    bool running;
    std::string playerName;
    TimeManager timeManager;

    GameManager();

    void showWelcome() const;
    void createPlayer();
    void processCurrentDay();
    void executeDailyAction();
    void calculateExam();
    void takeWeeklyRest();
    void showDayHeader() const;

public:
    static GameManager &getInstance();

    void startGame();
    void run();
    void endGame();

    GameManager(const GameManager &) = delete;
    GameManager &operator=(const GameManager &) = delete;
};

#endif
