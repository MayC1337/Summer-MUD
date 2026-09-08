#include "GameManager.h"
#include "ConsoleUI.h"
#include "../player/Player.h"

#include <iostream>
#include <map>
#include <sstream>
#include <set>

namespace
{
const char *getGradeLabel(int rank)
{
    static const char *const labels[] = {"?", "A", "B", "C", "D", "E"};
    return rank >= 1 && rank <= 5 ? labels[rank] : "?";
}
}

GameManager::GameManager()
    : running(false), player(nullptr), timeManager(TimeManager::DEFAULT_TOTAL_DAYS),
      saveManager("save.txt"), lastWeeklyScore(-1)
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
        ConsoleUI::boxTop("主菜单");
        ConsoleUI::boxLine("1. 新游戏    从高考倒计时第35天开始");
        ConsoleUI::boxLine("2. 继续游戏  读取最近一次自动存档");
        ConsoleUI::boxLine("3. 退出");
        ConsoleUI::boxBottom();
        std::cout << "请选择：";
        std::string choice;
        if (!std::getline(std::cin, choice))
        {
            return;
        }

        if (choice == "1")
        {
            createPlayer();
            timeManager.reset();
            lastWeeklyScore = -1;
            action.clearExitRequest();
            eventManager.setTriggeredEvents(std::set<std::string>());
            eventManager.setEventChoices(std::map<std::string, int>());
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
            action.clearExitRequest();
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
        if (timeManager.getDayOfWeek() == 1)
        {
            showChapterIntro();
        }
        showDailyNarration();
        processCurrentDay();

        if (player)
        {
            std::cout << "\n【今日结束状态】\n";
            player->showStatus();
        }

        const bool weekFinished = timeManager.isEndOfWeek();
        const int finishedWeek = timeManager.getCurrentWeek();
        timeManager.advanceDay();

        if (player && !saveManager.saveGame(*player, timeManager, eventManager))
        {
            std::cerr << "警告：自动存档失败。" << std::endl;
        }

        if (!running)
        {
            std::cout << "进度已保存，期待你下次继续。\n";
            break;
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
    if (!player)
    {
        return;
    }

    const ExamResult finalResult = exam.takeFinalExam(*player);
    std::cout << "\n================================" << std::endl;
    std::cout << playerName << "，35 天倒计时已经结束。" << std::endl;
    std::cout << "\n【最终状态】\n";
    player->showStatus();
    showExamDetails(finalResult);
    std::cout << "================================" << std::endl;

    showGrowthReport(finalResult);
    const std::string endingId = ending.judgeEnding(*player, finalResult, eventManager);
    ending.showEnding(endingId);
}

void GameManager::showWelcome() const
{
    ConsoleUI::showSplash();
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
    std::cout << "\n【初始状态】\n";
    player->showStatus();
}

void GameManager::showDayHeader() const
{
    const int elapsed = timeManager.getElapsedDays();
    const int filled = elapsed * 20 / timeManager.getTotalDays();
    static const char *const weather[] = {
        "晴", "多云", "微风", "小雨", "阴", "晴", "阵雨"};

    std::ostringstream dayLine;
    dayLine << "第 " << timeManager.getCurrentDay() << " / "
            << timeManager.getTotalDays() << " 天    第 "
            << timeManager.getCurrentWeek() << " 周·星期"
            << timeManager.getDayOfWeekName() << "    天气："
            << weather[(timeManager.getCurrentDay() - 1) % 7];
    std::ostringstream progressLine;
    progressLine << "高考倒计时  [";
    for (int i = 0; i < 20; ++i)
        progressLine << (i < filled ? "■" : "·");
    progressLine << "]  " << timeManager.getRemainingDays() << " 天";

    std::cout << '\n';
    ConsoleUI::boxTop("今日行程");
    ConsoleUI::boxLine(dayLine.str());
    ConsoleUI::boxLine(progressLine.str());
    ConsoleUI::boxBottom();
}

void GameManager::showChapterIntro() const
{
    static const char *const titles[] = {
        "第一章 重新出发", "第二章 压力升温", "第三章 模考风暴",
        "第四章 最后冲刺", "第五章 告别与选择"};
    static const char *const intros[] = {
        "倒计时第一次被写上黑板。三十五天看起来很长，又仿佛转眼就会过去。",
        "第一次周考已经贴在后墙。有人松了口气，也有人悄悄攥紧了笔。",
        "模拟考的脚步逼近，教室里的翻书声比往常更加急促。",
        "日历越来越薄，每一次选择都开始显得格外珍贵。",
        "最后一周到了。除了分数，还有许多话需要说，许多人需要告别。"};
    const int index = timeManager.getCurrentWeek() - 1;
    std::cout << "\n========== " << titles[index] << " ==========\n";
    std::cout << intros[index] << "\n";
}

void GameManager::showDailyNarration() const
{
    static const char *const narrations[35] = {
        "晨光落在新课表上，你写下了这个月的第一个计划。",
        "走廊里有人背着单词，脚步声从窗外匆匆掠过。",
        "黑板上的倒计时少了一格，数字第一次显得真实。",
        "午后的阳光有些刺眼，你揉了揉发酸的眼睛。",
        "第一周的最后一个学习日，书桌已经堆满了试卷。",
        "第一场周考发下来了，考场比平时安静得多。",
        "周日的风吹进房间，你终于有时间整理这一周。",
        "新的一周开始，熟悉的铃声里多了一点紧迫感。",
        "老师讲到重点时停顿了一下，全班同时拿起了笔。",
        "食堂里仍然热闹，话题却渐渐都变成了成绩。",
        "你发现睡眠和进度正在争夺同一段时间。",
        "窗边的同学撕下一页日历，离六月又近了一天。",
        "第二场周考到来，你开始看见选择累积出的差距。",
        "短暂的休息让呼吸慢下来，也让目标重新清晰。",
        "第三周，模拟考的阴影笼罩着每一张课桌。",
        "错题本越来越厚，但曾经陌生的题型也开始熟悉。",
        "一阵雨敲打窗户，晚自习教室仍然亮着灯。",
        "你偶尔怀疑是否来得及，却还是翻开了下一页。",
        "周五放学铃响，没有人像往常那样立刻离开。",
        "模拟考检验的不只是知识，还有此刻的心态。",
        "你给自己留出一个下午，重新安排最后两周。",
        "进入第四周，老师说现在最重要的是保持节奏。",
        "操场上传来口号声，教室里则只剩笔尖摩擦纸面的声音。",
        "你开始舍弃做不完的计划，专注真正重要的部分。",
        "疲惫悄悄累积，每一次休息也成了必要的选择。",
        "毕业照通知贴在门口，大家突然意识到离别也很近。",
        "最后一次正式周考，熟悉的座位显得有些不同。",
        "这个周日没有完全放松，你整理好了准考证和文具。",
        "最后一周，黑板上的数字已经变成了个位数。",
        "老师讲完最后一道例题，合上书时教室异常安静。",
        "同学们互相写下祝福，把紧张藏在玩笑后面。",
        "你重新翻看最初的计划，发现自己已经走了很远。",
        "最后一个学习日结束，晚霞把教学楼染成金色。",
        "最后一次周考更像一场预演，你学着与紧张相处。",
        "高考前的周日终于到来。夜色很轻，明天就在眼前。"};
    std::cout << "  “" << narrations[timeManager.getCurrentDay() - 1] << "”\n";
}

void GameManager::processCurrentDay()
{
    switch (timeManager.getCurrentDayType())
    {

    case TimeManager::DayType::Study:
        executeDailyAction();
        if (action.isExitRequested())
        {
            running = false;
            return;
        }
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
    std::cout << "\n========== 第 " << timeManager.getCurrentWeek()
              << " 周周考 ==========\n";
    showExamDetails(result);
    if (lastWeeklyScore >= 0)
    {
        const int change = result.score - lastWeeklyScore;
        std::cout << "与上周相比：" << (change >= 0 ? "+" : "")
                  << change << " 分\n";
    }
    lastWeeklyScore = result.score;

    player->modifyStat(StatType::Stress, 5);
    player->modifyStat(StatType::Intelligence, 1);
    std::cout << "周考结束：压力 +5，智力 +1。\n";
}

void GameManager::takeWeeklyRest()
{
    if (!player)
    {
        return;
    }

    player->modifyStat(StatType::Stress, -20);
    player->modifyStat(StatType::Stamina, 25);
    player->modifyStat(StatType::Health, 8);
    std::cout << "周日休息：压力 -20，体力 +25，健康 +8。" << std::endl;
}

void GameManager::triggerDailyEvent()
{
    if (player == nullptr)
    {
        return;
    }

    eventManager.triggerEvent(*player, timeManager.getCurrentDay());
}

void GameManager::showExamDetails(const ExamResult &result) const
{
    std::ostringstream subjects;
    subjects << "语文 " << result.chineseScore
             << "    数学 " << result.mathScore
             << "    英语 " << result.englishScore
             << "    理综 " << result.scienceScore;
    std::ostringstream total;
    total << "总分 " << result.score << "/100    等级 "
          << getGradeLabel(result.rank);
    ConsoleUI::boxTop("考试成绩");
    ConsoleUI::boxLine(subjects.str());
    ConsoleUI::boxLine(total.str());
    ConsoleUI::boxDivider();
    ConsoleUI::boxLine(result.feedback);
    ConsoleUI::boxBottom();
}

void GameManager::showGrowthReport(const ExamResult &result) const
{
    const Stats &stats = player->getStats();
    const StatType subjects[] = {
        StatType::Chinese, StatType::Math, StatType::English, StatType::Science};
    StatType strongest = subjects[0];
    for (StatType subject : subjects)
    {
        if (stats.get(subject) > stats.get(strongest))
            strongest = subject;
    }

    std::ostringstream scoreLine;
    scoreLine << "高考成绩：" << result.score << "/100";
    std::ostringstream subjectLine;
    subjectLine << "最强科目：" << to_string(strongest) << " "
                << stats.get(strongest);
    std::ostringstream stateLine;
    stateLine << "最终健康：" << stats.get(StatType::Health)
              << "    最终压力：" << stats.get(StatType::Stress);
    std::ostringstream eventLine;
    eventLine << "经历事件：" << eventManager.getTriggeredEvents().size()
              << " / 15";
    std::cout << '\n';
    ConsoleUI::boxTop("35天成长报告");
    ConsoleUI::boxLine(scoreLine.str());
    ConsoleUI::boxLine(subjectLine.str());
    ConsoleUI::boxLine(stateLine.str());
    ConsoleUI::boxLine(eventLine.str());
    ConsoleUI::boxBottom();
}
