#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "TimeManager.h"
#include "../action/Action.h"
#include "../event/EventManager.h"
#include "../exam/Exam.h"
#include "../exam/Ending.h"
#include "../save/SaveManager.h"

#include <memory>
#include <string>

class Player;

class GameManager
{
private:
    bool running;
    std::string playerName;
    std::unique_ptr<Player> player;
    TimeManager timeManager;
    Action action;
    EventManager eventManager;
    Exam exam;
    Ending ending;
    SaveManager saveManager;
    int lastWeeklyScore;

    GameManager();
    ~GameManager();

    void showWelcome() const;
    void createPlayer();
    void processCurrentDay();
    void executeDailyAction();
    void calculateExam();
    void takeWeeklyRest();
    void showDayHeader() const;
    void showChapterIntro() const;
    void showDailyNarration() const;
    void showExamDetails(const ExamResult &result) const;
    void showGrowthReport(const ExamResult &result) const;
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
