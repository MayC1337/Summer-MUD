#include "GameManager.h"
#include "ConsoleUI.h"
#include "../command/CommandParser.h"
#include "../player/Player.h"
#include "../player/Inventory.h"

#include <iostream>
#include <map>
#include <sstream>
#include <set>
#include <algorithm>
#include <cctype>

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
      saveManager("save.txt"), npcs(NPC::createCampusNPCs())
{
}

GameManager::~GameManager() = default;

bool GameManager::saveProgress()
{
    progress.location = world.currentRoom().id;
    progress.pendingEvent = eventManager.getPendingEvent();
    if (progress.stage == GameProgress::Stage::Action)
        progress.pendingChoices = CommandParser::actionChoices();
    if (!saveManager.saveGame(*player, timeManager, eventManager, progress))
    {
        ConsoleUI::message("保存失败：未替换旧存档。请检查目录权限或磁盘空间。", true);
        return false;
    }
    return true;
}

void GameManager::showPeople() const
{
    std::cout << "这里的人物：";
    bool present = false;
    std::string example;
    for (const auto& npc : npcs)
        if (npc.isPresent(world.currentRoom().id, progress.period))
        {
            std::cout << npc.getName() << "(" << npc.getId() << ")  ";
            if (!present) example = npc.getName();
            present = true;
        }
    if (!present) std::cout << "暂时没有可交谈的人。";
    std::cout << '\n';
    if (present) std::cout << "输入「交谈 人物名」即可聊天，不耗行动，例如：交谈 " << example << "。\n";
}

void GameManager::showCampusPeople() const
{
    std::cout << "【当前时段 · 地点人物】\n";
    for (const auto& entry : world.getRooms())
    {
        std::cout << entry.second.name << "：";
        bool present = false;
        for (const auto& npc : npcs)
            if (npc.isPresent(entry.first, progress.period))
            {
                std::cout << (present ? "、" : "") << npc.getName();
                present = true;
            }
        std::cout << (present ? "\n" : "暂无人物\n");
    }
    std::cout << "到达后输入「交谈 人物名」；晚间可移动，聊天不耗行动。\n";
}

void GameManager::showDailySummary() const
{
    const Stats& stats = player->getStats();
    ConsoleUI::boxTop("今日小结");
    ConsoleUI::boxLine("体力 " + std::to_string(stats.get(StatType::Stamina)) +
        "  健康 " + std::to_string(stats.get(StatType::Health)) +
        "  压力 " + std::to_string(stats.get(StatType::Stress)) +
        "  金钱 " + std::to_string(player->getMoney()));
    ConsoleUI::boxLine("学科能力：语文 " + std::to_string(stats.get(StatType::Chinese)) +
        "  数学 " + std::to_string(stats.get(StatType::Math)) +
        "  英语 " + std::to_string(stats.get(StatType::English)) +
        "  理综 " + std::to_string(stats.get(StatType::Science)));
    ConsoleUI::boxLine(progress.lastWeeklyScore < 0 ? "首次周测：第6天（周六），满分750。" :
        "最近周测：" + std::to_string(progress.lastWeeklyScore) + " / 750");
    ConsoleUI::boxLine("学科能力满值100，不是考试分数；输入「状态」查看完整属性。");
    ConsoleUI::boxBottom();
}

bool GameManager::handleCommand(const std::string& command)
{
    std::istringstream parser(command);
    std::string verb;
    parser >> verb;
    std::transform(verb.begin(), verb.end(), verb.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    std::string argument;
    std::getline(parser >> std::ws, argument);
    if (verb == "help" || verb == "帮助")
    {
        ConsoleUI::boxTop("命令帮助 · 查看信息不耗行动");
        ConsoleUI::boxLine("status/状态  inventory/背包  time/时间");
        ConsoleUI::boxLine("map/地图  look/观察  who/人物");
        ConsoleUI::boxLine("talk 林晓 / 交谈 林晓（也可使用英文人物ID）");
        ConsoleUI::boxLine("save/存档  quit/退出（保存成功后退出）");
        ConsoleUI::boxLine("晚间主菜单：north/south/east/west 或 北/南/东/西");
        ConsoleUI::boxLine("到达后选择6进行当地活动；1~5可沿路线直达。");
        ConsoleUI::boxBottom();
    }
    else if (verb == "status" || verb == "状态") player->showStatus();
    else if (verb == "inventory" || verb == "背包") player->getInventory().showItems();
    else if (verb == "time" || verb == "时间")
    {
        showDayHeader();
        const char* periods[] = {"晨间", "午间", "下午", "晚间"};
        std::cout << "当前时段：" << periods[progress.period] << "；地点：" << world.currentRoom().name << '\n';
    }
    else if (verb == "map" || verb == "地图") { world.show(); showCampusPeople(); }
    else if (verb == "look" || verb == "观察") { world.look(); showPeople(); }
    else if (verb == "who" || verb == "人物") showPeople();
    else if (verb == "talk" || verb == "交谈")
    {
        for (const auto& npc : npcs)
            if ((npc.getName() == argument || npc.getId() == argument) &&
                npc.isPresent(world.currentRoom().id, progress.period))
            {
                const std::string key = npc.getId() + "@" + std::to_string(timeManager.getCurrentDay());
                npc.talk(*player, eventManager, progress.npcConversations.insert(key).second);
                return true;
            }
        std::cout << "这个人物当前不在这里。输入 who 查看人物及ID。\n";
    }
    else if (verb == "save" || verb == "存档" || verb == "quit" || verb == "退出")
    {
        if (!argument.empty()) { std::cout << "该命令不需要参数。\n"; return true; }
        if (saveProgress())
        {
            ConsoleUI::message("进度已保存：日期、时段、地点和未完成阶段均已记录。");
            if (verb == "quit" || verb == "退出") throw InputInterrupted(false);
        }
        else std::cout << "仍留在当前菜单，可以修复目录后重试或继续游玩。\n";
    }
    else
    {
        const std::map<std::string, std::string> directions = {
            {"north", "north"}, {"south", "south"}, {"east", "east"}, {"west", "west"},
            {"北", "north"}, {"南", "south"}, {"东", "east"}, {"西", "west"}};
        const auto direction = directions.find(verb);
        if (direction == directions.end()) return false;
        if (!argument.empty()) { std::cout << "方向命令不需要参数。\n"; return true; }
        if (progress.stage != GameProgress::Stage::Action || progress.period != 3 ||
            !CommandParser::actionChoices().empty())
            std::cout << "自由移动在晚间主菜单开放；当前请先完成正在进行的选择。\n";
        else world.move(direction->second);
    }
    return true;
}

GameManager &GameManager::getInstance()
{
    static GameManager instance;
    return instance;
}

void GameManager::startGame()
{
    action.setWorld(&world);
    // 地图只通知到达，由调度器展示人物，不让地图依赖 NPC 模块。
    world.setArrivalHandler([this]() { showPeople(); });
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
            progress = GameProgress{};
            world.setLocation(progress.location);
            eventManager.setPendingEvent("");
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
            if (!saveManager.loadGame(*player, timeManager, eventManager, progress))
            {
                player.reset();
                std::cout << "存档损坏或版本不兼容，读取失败。\n";
                continue;
            }

            world.setLocation(progress.location);
            eventManager.setPendingEvent(progress.pendingEvent);
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

    CommandParser::setCommandHandler([this](const std::string& command) { return handleCommand(command); });
    std::cout << "输入 help 查看命令；任意选择处可 save 存档、quit 保存退出。\n";
    std::cout << "玩法：每天选行动，每周六看周测分数，35天后参加高考。\n"
              << "学习提高学科能力，累了就休息；到达地点会自动提示可交谈人物。\n";
    running = true;
    run();
    CommandParser::setCommandHandler({});
}

void GameManager::run()
{
    if (!timeManager.isFinished() && progress.stage != GameProgress::Stage::DayStart)
    {
        showDayHeader();
        std::cout << "恢复地点：" << world.currentRoom().name
                  << "；从未完成的阶段继续。\n";
    }
    while (running && !timeManager.isFinished())
    {
        try
        {
            processCurrentDay();
            if (running) saveProgress();
        }
        catch (const InputInterrupted& interruption)
        {
            if (interruption.endOfInput)
            {
                std::cout << "\n输入已结束，停止推进时间。\n";
                saveProgress();
            }
            CommandParser::endActions();
            action.clearExitRequest();
            running = false;
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
    ending.showEnding(endingId, finalResult);
    ConsoleUI::boxTop("六月 · 录取通知书");
    ConsoleUI::boxLine("考生：" + playerName);
    ConsoleUI::boxLine("高考成绩：" + std::to_string(finalResult.score) + " / 750");
    ConsoleUI::boxLine("模拟去向：" + finalResult.university);
    ConsoleUI::boxBottom();
    eventManager.showMemories();
    ConsoleUI::boxTop("下一站");
    ConsoleUI::boxLine("           ( ^.^ )      __");
    ConsoleUI::boxLine("           /|___|>     |[]|   -->");
    ConsoleUI::boxLine("            /   \\      oo");
    if (finalResult.university.find("中国海洋大学") != std::string::npos)
    {
        ConsoleUI::boxLine("    ~~~~~      ~~~~~      ~~~~~");
        ConsoleUI::boxLine("海风翻开新书页，海鸥掠过青岛的天空。");
        ConsoleUI::boxLine("下一次铃响，你将在中国海洋大学走进教室。");
    }
    else if (finalResult.score >= 450)
        ConsoleUI::boxLine("拖着行李箱走进新校园，新的故事等你落笔。");
    else ConsoleUI::boxLine("你重新打开地图，人生还有很多条可以出发的路。");
    ConsoleUI::boxLine("铃声响过了，你的人生还在继续。");
    ConsoleUI::boxBottom();
    ConsoleUI::waitForExit("输入 0 结束这段高三旅程：");
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
    using Stage = GameProgress::Stage;
    switch (progress.stage)
    {
    case Stage::DayStart:
        showDayHeader();
        if (timeManager.getDayOfWeek() == 1) showChapterIntro();
        showDailyNarration();
        processWeeklyMilestone();
        progress.stage = timeManager.getCurrentDayType() == TimeManager::DayType::Study ?
            Stage::Routine : Stage::SpecialDay;
        break;
    case Stage::Goal:
        // 旧存档可能停在目标选择页，直接跳过，不再询问或结算目标。
        progress.stage = Stage::Routine;
        break;
    case Stage::Routine:
    {
        int repeat = 1;
        if (!progress.previousActions[0].empty())
        {
            std::cout << "1. 今天自己安排\n2. 沿用上个学习日的普通行动\n";
            if (!CommandParser::readChoice(repeat, 1, 2)) return;
        }
        progress.repeat = repeat == 2;
        progress.period = 0;
        world.setLocation("gate");
        progress.stage = Stage::Action;
        break;
    }
    case Stage::Action:
        executeDailyAction();
        break;
    case Stage::Story:
        eventManager.triggerStory(*player, timeManager.getCurrentDay(), progress.period);
        ++progress.period;
        if (progress.period == 4)
        {
            progress.period = 3;
            progress.stage = Stage::DailyEvent;
        }
        else
        {
            const char* locations[] = {"gate", "canteen", "classroom", "gate"};
            world.setLocation(locations[progress.period]);
            progress.stage = Stage::Action;
        }
        break;
    case Stage::SpecialDay:
        if (timeManager.getCurrentDayType() == TimeManager::DayType::Exam)
        {
            world.setLocation("classroom");
            calculateExam();
            progress.stage = Stage::FinishDay;
            // 确认前先记录已结算阶段，退出/断流后不能重复考试或领取收益。
            saveProgress();
            std::cout << "成绩单已显示。输入1继续（也可输入 quit 保存退出）：";
            int confirmation = 0;
            CommandParser::readChoice(confirmation, 1, 1);
        }
        else
        {
            world.setLocation("home");
            takeWeeklyRest();
            progress.stage = Stage::DailyEvent;
        }
        break;
    case Stage::DailyEvent:
        triggerDailyEvent();
        progress.stage = Stage::FinishDay;
        break;
    case Stage::FinishDay:
        showDailySummary();
        timeManager.advanceDay();
        progress.stage = Stage::DayStart;
        progress.period = 0;
        progress.repeat = false;
        progress.pendingChoices.clear();
        world.setLocation("gate");
        break;
    }
}

void GameManager::executeDailyAction()
{
    action.setDayOfWeek(timeManager.getDayOfWeek());
    action.setTime(static_cast<ActionTime>(progress.period));
    const auto index = static_cast<std::size_t>(progress.period);
    const auto inputs = !progress.pendingChoices.empty() ? progress.pendingChoices :
        progress.repeat ? progress.previousActions[index] : std::vector<int>{};
    if (progress.period == 3 && progress.repeat && progress.pendingChoices.empty() &&
        !inputs.empty() && inputs.front() == 6)
        world.travelTo(progress.previousEveningLocation);
    std::cout << "\n当前位置：" << world.currentRoom().name << '\n';
    showPeople();
    if (progress.period == 3) showCampusPeople();
    CommandParser::beginActions(inputs);
    action.executeDailyAction(*player);
    auto choices = CommandParser::endActions();
    if (action.isExitRequested())
    {
        action.clearExitRequest();
        progress.pendingChoices.clear();
        progress.repeat = false;
        if (saveProgress())
        {
            ConsoleUI::message("进度已保存，下次从当前晚间继续。");
            throw InputInterrupted(false);
        }
        std::cout << "退出已取消。仍在当前时段，可重试 save 或继续游戏。\n";
        return;
    }
    progress.previousActions[index] = std::move(choices);
    if (progress.period == 3) progress.previousEveningLocation = world.currentRoom().id;
    progress.pendingChoices.clear();
    progress.stage = GameProgress::Stage::Story;
}

void GameManager::processWeeklyMilestone()
{
    const int day = timeManager.getCurrentDay();
    const char* titles[] = {"写下心愿", "班级小组挑战", "全校模拟考动员", "毕业照与留言册", "整理最后一张书桌"};
    const char* descriptions[] = {
        "你在日记第一页留出一行，准备写下六月的目标。",
        "老师把复习题分给小组，每个人都可以贡献一点力量。",
        "模考检验准备，也考验如何面对暂时的失利。",
        "同学们互借笔写留言，窗外的梧桐已经长得很密。",
        "抽屉里有旧试卷、糖纸和朋友递来的便签。"};
    if ((day - 1) % 7 != 0) return;
    const int week = (day - 1) / 7;
    const std::string id = "milestone_" + std::to_string(week + 1);
    if (eventManager.hasTriggered(id)) return;
    ConsoleUI::boxTop(titles[week]);
    ConsoleUI::boxLine(descriptions[week]);
    ConsoleUI::boxBottom();
    if (week == 0)
        std::cout << "1. 向海出发：中国海洋大学（目标615分）\n2. 冲刺更高的山峰（目标680分）\n3. 稳住自己的节奏（目标540分）\n";
    else if (week == 1)
        std::cout << "1. 负责讲解（智力 +1，压力 +2）\n2. 协调合作（情商 +2，压力 -2）\n3. 独立整理资料（理综 +2，体力 -3）\n";
    else if (week == 2)
        std::cout << "1. 梳理易错点（智力 +1）\n2. 调整作息（体力 +4）\n3. 和同桌互相鼓励（压力 -4）\n";
    else if (week == 3)
        std::cout << "1. 写下感谢（情商 +2）\n2. 给未来留一句话（压力 -3）\n3. 收好全班合照（压力 -2）\n";
    else std::cout << "1. 收藏旧便签（压力 -3）\n2. 把笔记送给学弟妹（情商 +2）\n3. 整理文具，早点睡（体力 +4）\n";
    int choice = 0;
    if (!CommandParser::readChoice(choice, 1, 3)) return;
    if (week == 1)
    {
        player->modifyStat(choice == 1 ? StatType::Intelligence : choice == 2 ? StatType::EQ : StatType::Science, choice == 1 ? 1 : 2);
        player->modifyStat(choice == 3 ? StatType::Stamina : StatType::Stress, choice == 1 ? 2 : choice == 2 ? -2 : -3);
        if (eventManager.getEventChoice("desk_2") == 1 && choice != 3)
        {
            player->modifyStat(StatType::Science, 1);
            std::cout << "林晓接过你的讲解，你们上次合作的默契派上用场。理综 +1。\n";
        }
    }
    if (week == 2) player->modifyStat(choice == 1 ? StatType::Intelligence : choice == 2 ? StatType::Stamina : StatType::Stress, choice == 1 ? 1 : choice == 2 ? 4 : -4);
    if (week == 3) player->modifyStat(choice == 1 ? StatType::EQ : StatType::Stress, choice == 1 ? 2 : choice == 2 ? -3 : -2);
    if (week == 4) player->modifyStat(choice == 1 ? StatType::Stress : choice == 2 ? StatType::EQ : StatType::Stamina, choice == 1 ? -3 : choice == 2 ? 2 : 4);
    eventManager.markTriggered(id);
    auto choices = eventManager.getEventChoices();
    choices[id] = choice;
    eventManager.setEventChoices(choices);
    std::cout << "这一页已经写进了你的六月记忆。\n";
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
    const char* examNarrations[] = {
        "第一次周考：先看清自己的起点，不急着否定自己。",
        "小组挑战之后，今天试着独立运用学到的方法。",
        "全校模拟考：座位重新编排，请把这次当作正式预演。",
        "最后阶段的诊断：优先找出还能修补的失分点。",
        "考前适应练习：不再追求题量，带着熟悉的节奏进考场。"};
    std::cout << examNarrations[timeManager.getCurrentWeek() - 1] << '\n';
    showExamDetails(result);
    if (progress.lastWeeklyScore >= 0)
    {
        const int change = result.score - progress.lastWeeklyScore;
        std::cout << "与上周相比：" << (change >= 0 ? "+" : "")
                  << change << " 分\n";
    }
    progress.lastWeeklyScore = result.score;

    // 按得分率比较，不能把满分300的理综与满分150的科目直接比较原始分。
    const int scores[] = {result.chineseScore, result.mathScore, result.englishScore, result.scienceScore};
    const int maximums[] = {150, 150, 150, 300};
    const char* names[] = {"语文", "数学", "英语", "理综"};
    int weakest = 0;
    for (int i = 1; i < 4; ++i)
        if (scores[i] * maximums[weakest] < scores[weakest] * maximums[i]) weakest = i;
    std::cout << "下周建议：优先复习" << names[weakest] << "（本次得分率 "
              << scores[weakest] * 100 / maximums[weakest] << "%），兼顾休息。\n"
              << "周测按当前能力与状态计算，含少量波动，不等于最终高考成绩。\n";

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
    std::ostringstream languages;
    languages << "语文 " << result.chineseScore << "/150"
              << "    数学 " << result.mathScore << "/150";
    std::ostringstream comprehensive;
    comprehensive << "英语 " << result.englishScore << "/150"
                  << "    理综 " << result.scienceScore << "/300";
    std::ostringstream total;
    total << "总分 " << result.score << "/750    等级 "
          << getGradeLabel(result.rank);
    ConsoleUI::boxTop("考试成绩");
    ConsoleUI::boxLine(languages.str());
    ConsoleUI::boxLine(comprehensive.str());
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
    scoreLine << "高考成绩：" << result.score << "/750";
    std::ostringstream subjectLine;
    subjectLine << "最强科目：" << to_string(strongest) << " "
                << stats.get(strongest);
    std::ostringstream stateLine;
    stateLine << "最终健康：" << stats.get(StatType::Health)
              << "    最终压力：" << stats.get(StatType::Stress);
    std::ostringstream eventLine;
    eventLine << "经历故事与事件：" << eventManager.getTriggeredEvents().size();
    std::cout << '\n';
    ConsoleUI::boxTop("35天成长报告");
    ConsoleUI::boxLine(scoreLine.str());
    ConsoleUI::boxLine(subjectLine.str());
    ConsoleUI::boxLine(stateLine.str());
    ConsoleUI::boxLine(eventLine.str());
    const int goal = eventManager.getEventChoice("milestone_1");
    if (goal != 0)
    {
        const int target = goal == 1 ? 615 : goal == 2 ? 680 : 540;
        ConsoleUI::boxLine("月初心愿分数：" + std::to_string(target) +
            (result.score >= target ? "  已达到" : "  尚有距离，也有下一段路"));
    }
    ConsoleUI::boxBottom();
}
