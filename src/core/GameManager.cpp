#include "GameManager.h"

#include <iostream>

GameManager::GameManager()
    : running(false), player(nullptr), timeManager(TimeManager::DEFAULT_TOTAL_DAYS)
{
}

GameManager &GameManager::getInstance()
{
    static GameManager instance;
    return instance;
}

void GameManager::startGame()
{
    showWelcome();
    createPlayer();

    eventManager.loadEvents();

    timeManager.reset();
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

    // 外部模块依赖：Player 实现接入后在这里创建对象并赋给 player。
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
    // Action 模块接入点：Action::execute(player) 应在这里执行并修改玩家属性。
    std::cout << "日常行动：完成今天的默认学习计划。" << std::endl;
}

void GameManager::calculateExam()
{
    // Exam 模块接入点：Exam::calculate(player.getStats()) 应在这里计算并返回成绩。
    std::cout << "阶段考试：本周学习成果进入结算。" << std::endl;
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
