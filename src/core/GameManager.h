#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "TimeManager.h"
#include "GameProgress.h"
#include "../world/CampusMap.h"
#include "../npc/NPC.h"
#include "../action/Action.h"
#include "../event/EventManager.h"
#include "../exam/Exam.h"
#include "../exam/Ending.h"
#include "../save/SaveManager.h"

#include <memory>
#include <array>
#include <vector>
#include <string>

class Player;

// 游戏总调度器：协调时间、行动、剧情、存档和结局的阶段流转。
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
    GameProgress progress; // 可序列化的流程状态；实体状态由其他成员分别维护。
    CampusMap world;       // 校园地点与移动关系。
    std::vector<NPC> npcs;
    bool saveProgress();
    bool handleCommand(const std::string& command);
    void showPeople() const;
    void showCampusPeople() const;
    void showDailySummary() const;
    void processWeeklyMilestone();

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
