#include "GameManager.h"
#include "../player/Player.h"

#include <iostream>
#include <set>

GameManager::GameManager()
    : running(false), player(nullptr), timeManager(TimeManager::DEFAULT_TOTAL_DAYS),
      saveManager("save.txt")
{
}

GameManager::~GameManager() = default;

GameManager &GameManager::getInstance()
{
    static GameManager instance;
    return instance;
}

void GameManager::startGame()
{
    showWelcome();
    eventManager.loadEvents();

    while (true)
    {
        std::cout << "1. 新游戏\n2. 继续游戏\n3. 退出\n请选择：";
        std::string choice;
        if (!std::getline(std::cin, choice))
        {
            return;
        }

        if (choice == "1")
        {
            createPlayer();
            timeManager.reset();
            eventManager.setTriggeredEvents(std::set<std::string>());
            break;
        }

        if (choice == "2")
        {
            if (!saveManager.hasSave())
            {
                std::cout << "当前没有存档。\n";
                continue;
            }

            player.reset(new Player("无名考生"));
            if (!saveManager.loadGame(*player, timeManager, eventManager))
            {
                player.reset();
                std::cout << "存档损坏或版本不兼容，读取失败。\n";
                continue;
            }

            playerName = player->getName();
            std::cout << "读档成功：" << playerName << "，已度过 "
                      << timeManager.getElapsedDays() << " 天。\n";
            break;
        }

        if (choice == "3")
        {
            std::cout << "游戏已退出。\n";
            return;
        }

        std::cout << "无效选择，请重新输入。\n";
    }

    running = true;
    run();
}

void GameManager::run()
{
    while (running && !timeManager.isFinished())
    {
        showDayHeader();
        processCurrentDay();

        const bool weekFinished = timeManager.isEndOfWeek();
        const int finishedWeek = timeManager.getCurrentWeek();
        timeManager.advanceDay();

        if (player && !saveManager.saveGame(*player, timeManager, eventManager))
        {
            std::cerr << "警告：自动存档失败。" << std::endl;
        }

        if (weekFinished)
        {
            std::cout << "第 " << finishedWeek << " 周结束。" << std::endl;
        }
    }

    endGame();
}

void GameManager::endGame()
{
    if (!running)
    {
        return;
    }

    running = false;
    std::cout << "\n================================" << std::endl;
    std::cout << playerName << "，35 天倒计时已经结束。" << std::endl;
    std::cout << "高考与最终结局模块将在后续阶段接入。" << std::endl;
    std::cout << "================================" << std::endl;
}

void GameManager::showWelcome() const
{
    std::cout << "================================" << std::endl;
    std::cout << "          Summer-MUD" << std::endl;
    std::cout << "         高三人生模拟" << std::endl;
    std::cout << "================================" << std::endl;
}

void GameManager::createPlayer()
{
    std::cout << "请输入玩家姓名：";
    std::getline(std::cin, playerName);

    if (playerName.empty())
    {
        playerName = "无名考生";
    }

    player.reset(new Player(playerName));
    std::cout << "欢迎你，" << playerName << "！高考倒计时 35 天。" << std::endl;
}

void GameManager::showDayHeader() const
{
    std::cout << "\n[第 " << timeManager.getCurrentDay() << "/"
              << timeManager.getTotalDays() << " 天 | 第 "
              << timeManager.getCurrentWeek() << " 周 | 星期"
              << timeManager.getDayOfWeekName() << " | 距高考 "
              << timeManager.getRemainingDays() << " 天]" << std::endl;
}

void GameManager::processCurrentDay()
{
    switch (timeManager.getCurrentDayType())
    {

    case TimeManager::DayType::Study:
        executeDailyAction();
        triggerDailyEvent();
        break;

    case TimeManager::DayType::Exam:
        calculateExam();
        break;

    case TimeManager::DayType::Rest:
        takeWeeklyRest();
        triggerDailyEvent();
        break;
    }
}

void GameManager::executeDailyAction()
{
    if (!player)
    {
        return;
    }

    const ActionTime daySchedule[] = {
        ActionTime::Morning,
        ActionTime::Noon,
        ActionTime::Afternoon,
        ActionTime::Evening};

    for (ActionTime actionTime : daySchedule)
    {
        action.setTime(actionTime);
        action.executeDailyAction(*player);
    }
}

void GameManager::calculateExam()
{
    if (!player)
    {
        return;
    }

    const ExamResult result = exam.takeWeeklyExam(*player);
    std::cout << "阶段考试：成绩 " << result.score
              << "，排名 " << result.rank << "。\n"
              << result.feedback << std::endl;
}

void GameManager::takeWeeklyRest()
{
    std::cout << "周日休息：恢复状态，准备下一周。" << std::endl;
}

void GameManager::triggerDailyEvent()
{
    if (player == nullptr)
    {
        return;
    }

    eventManager.triggerEvent(
        *player);
}
