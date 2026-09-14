#include "Action.h"
#include "../world/CampusMap.h"
#include "../command/CommandParser.h"
#include "../core/ConsoleUI.h"
#include "../player/Inventory.h"
#include "../player/Item.h"
#include "../player/Player.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <random>

// 行动对象不拥有玩家或地图；二者均由外部调度器在每次行动时提供。
Action::Action()
    : currentTime(ActionTime::Morning), currentDayOfWeek(1), exitRequested(false)
{
}

void Action::setWorld(CampusMap* map) { world = map; }

void Action::setDayOfWeek(int dayOfWeek)
{
    currentDayOfWeek = std::clamp(dayOfWeek, 1, 7);
}

void Action::setTime(ActionTime time)
{
    currentTime = time;
}

ActionTime Action::getTime() const
{
    return currentTime;
}

bool Action::isExitRequested() const
{
    return exitRequested;
}

void Action::clearExitRequest()
{
    exitRequested = false;
}

void Action::modifyStat(Player& player, StatType type, int delta)
{
    Stats& stats = player.getStats();

    int currentValue = stats.get(type);
    int newValue = currentValue + delta;

    if (type == StatType::Health && newValue < 0)
        newValue = 0;

    if (type == StatType::Health && newValue > 100)
        newValue = 100;

    if (type == StatType::Stress && newValue < 0)
        newValue = 0;

    stats.set(type, newValue);
}

bool Action::hasItem(Player& player, const std::string& itemName)
{
    return player.getInventory().hasItem(itemName);
}

bool Action::checkCaught(int probability)
{
    static std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(1, 100);
    return distribution(generator) <= probability;
}

double Action::getStudyMultiplier(Player& player)
{
    Stats& stats = player.getStats();

    int stress = stats.get(StatType::Stress);
    int health = stats.get(StatType::Health);

    double multiplier = 1.0;

    if (stress >= 90)
        multiplier *= 0.5;
    else if (stress >= 70)
        multiplier *= 0.75;

    if (health <= 20)
        multiplier *= 0.5;

    if (stats.get(StatType::Stamina) <= 15)
        multiplier *= 0.5;

    return multiplier;
}

StatType Action::getScheduledSubject() const
{
    const StatType subjects[] = {
        StatType::Chinese, StatType::Math, StatType::English, StatType::Science};
    const int lessonIndex = currentDayOfWeek - 1;
    return subjects[lessonIndex % 4];
}

int Action::calculateStudyGain(Player& player, StatType subject, int baseGain)
{
    // 学科越接近上限，收益越低；乘数同时反映智力和体力/压力状态。
    const int ability = player.getStats().get(subject);
    int adjustedBase = baseGain;
    if (ability >= 90)
        adjustedBase = 1;
    else if (ability >= 80)
        adjustedBase = std::min(adjustedBase, 2);
    else if (ability >= 60)
        adjustedBase = std::min(adjustedBase, 3);

    const int gain = static_cast<int>(std::lround(
        adjustedBase * getStudyMultiplier(player)));
    return std::max(1, gain);
}

StatType Action::chooseSubject()
{
    std::cout << "\n请选择学习科目：\n";
    std::cout << "1. 语文\n";
    std::cout << "2. 数学\n";
    std::cout << "3. 英语\n";
    std::cout << "4. 理综\n";

    int choice = 0;
    if (!CommandParser::readChoice(choice, 1, 4))
    {
        return StatType::Math;
    }

    switch (choice)
    {
    case 1: return StatType::Chinese;
    case 2: return StatType::Math;
    case 3: return StatType::English;
    case 4: return StatType::Science;
    default:
        std::cout << "输入无效，默认选择数学。\n";
        return StatType::Math;
    }
}

void Action::attendClass(Player& player)
{
    std::cout << "\n========== 认真听课 ==========\n";

    const StatType subject = getScheduledSubject();
    const double multiplier = getStudyMultiplier(player);
    const int gain = calculateStudyGain(player, subject, 3);
    const bool gainedInsight = currentTime == ActionTime::Afternoon &&
        player.getStats().get(StatType::Intelligence) < 75;

    modifyStat(player, subject, gain);
    if (gainedInsight)
        modifyStat(player, StatType::Intelligence, 1);
    modifyStat(player, StatType::Stress, 4);
    modifyStat(player, StatType::Stamina, -3);

    std::cout << "今天这节是" << to_string(subject) << "课，你跟着老师梳理了重点。\n";
    std::cout << to_string(subject) << " +" << gain;
    if (gainedInsight) std::cout << "，理解力积累使智力 +1";
    std::cout << '\n';
    std::cout << "压力 +4，体力 -3\n";

    if (multiplier < 1.0)
        std::cout << "由于你的状态不佳，学习效率下降了。\n";
}

void Action::sleepInClass(Player& player)
{
    std::cout << "\n========== 上课睡觉 ==========\n";

    modifyStat(player, StatType::Stress, -12);
    modifyStat(player, StatType::Stamina, 10);
    modifyStat(player, StatType::Health, 2);

    std::cout << "你趁老师不注意睡了一会儿。\n";
    std::cout << "压力 -12，体力 +10，健康 +2\n";
    std::cout << "本次没有获得学习收益。\n";
}

void Action::selfStudy(Player& player)
{
    // 专项练习允许玩家在“基础、错题、难题”之间用风险换取更高收益。
    std::cout << "\n========== 自习 ==========\n";

    StatType subject = chooseSubject();
    std::cout << "1. 补基础（薄弱科收益高；压力 +6，体力 -5）\n"
              << "2. 整理错题（科目 +1~3，智力 +1；压力 +5，体力 -4）\n"
              << "3. 挑战难题（科目 +0~6；压力 +12，体力 -10）\n";
    int method = 0;
    if (!CommandParser::readChoice(method, 1, 3)) return;
    const double multiplier = getStudyMultiplier(player);
    const int ability = player.getStats().get(subject);
    int gain = calculateStudyGain(player, subject, method == 2 ? 3 : 5);
    if (method == 1 && ability >= 80) gain = 1;
    if (method == 3)
    {
        const bool solved = checkCaught(ability >= 70 ? 65 : 35);
        gain = solved ? calculateStudyGain(player, subject, 6) : 0;
        std::cout << (solved ? "关键一步终于被你突破了。\n" : "这道难题暂时没有解开，下次先检查基础。\n");
    }
    const int stressCost = method == 1 ? 6 : method == 2 ? 5 : 12;
    const int staminaCost = method == 1 ? 5 : method == 2 ? 4 : 10;

    modifyStat(player, subject, gain);
    if (method == 2) modifyStat(player, StatType::Intelligence, 1);
    modifyStat(player, StatType::Stress, stressCost);
    modifyStat(player, StatType::Stamina, -staminaCost);

    std::cout << "你完成了一次自习。\n";
    std::cout << "对应科目熟练度 +" << gain << "\n";
    std::cout << "压力 +" << stressCost << "，体力 -" << staminaCost << '\n';
    if (method == 2) std::cout << "智力 +1，整理思路有助于减少周考波动。\n";
    if (multiplier < 1.0)
        std::cout << "状态不佳让本次自习的效率打了折扣。\n";
}

void Action::takeNap(Player& player)
{
    std::cout << "\n========== 睡午觉 ==========\n";

    modifyStat(player, StatType::Stress, -10);
    modifyStat(player, StatType::Stamina, 15);
    modifyStat(player, StatType::Health, 2);

    std::cout << "你睡了一觉午觉。\n";
    std::cout << "压力 -10，体力 +15，健康 +2\n";
}

void Action::earlyRest(Player& player)
{
    std::cout << "\n========== 提前休息 ==========\n";

    modifyStat(player, StatType::Stress, -25);
    modifyStat(player, StatType::Stamina, 20);
    modifyStat(player, StatType::Health, 5);

    std::cout << "你决定今晚早点睡觉。\n";
    std::cout << "压力 -25，体力 +20，健康 +5\n";
}

void Action::study(Player& player)
{
    if (currentTime == ActionTime::Morning ||
        currentTime == ActionTime::Afternoon)
    {
        std::cout << "\n========== 学习 ==========\n";
        std::cout << "1. 认真听课（按课表，科目 +1~3，压力 +4，体力 -3）\n";
        std::cout << "2. 专项练习（自由选科：补基础 / 错题 / 难题）\n";

        int choice = 0;
        if (!CommandParser::readChoice(choice, 1, 2))
        {
            return;
        }

        if (choice == 1)
            attendClass(player);
        else if (choice == 2)
            selfStudy(player);
        else
            std::cout << "无效选择。\n";

        return;
    }

    if (currentTime == ActionTime::Evening)
    {
        std::cout << "\n你来到图书馆学习。\n";
        selfStudy(player);
        return;
    }

    std::cout << "现在不是适合学习的时间。\n";
}

void Action::rest(Player& player)
{
    if (currentTime == ActionTime::Morning ||
        currentTime == ActionTime::Afternoon)
    {
        sleepInClass(player);
        return;
    }

    if (currentTime == ActionTime::Noon)
    {
        takeNap(player);
        return;
    }

    if (currentTime == ActionTime::Evening)
    {
        earlyRest(player);
        return;
    }

    std::cout << "当前无法休息。\n";
}

void Action::eatSnack(Player& player)
{
    const std::string itemName = "snack";

    if (!hasItem(player, itemName))
    {
        std::cout << "你没有零食。\n";
        return;
    }

    player.getInventory().removeItem(itemName);
    modifyStat(player, StatType::Stress, -8);

    std::cout << "你偷偷吃了一袋零食。\n";
    std::cout << "压力 -8\n";
}

void Action::playMP4(Player& player)
{
    const std::string itemName = "mp4";

    if (!hasItem(player, itemName))
    {
        std::cout << "你没有MP4。\n";
        return;
    }

    modifyStat(player, StatType::Stress, -10);

    std::cout << "你偷偷玩了一会儿MP4。\n";
    std::cout << "压力 -10\n";

    if (checkCaught(10))
    {
        modifyStat(player, StatType::Stress, 50);
        player.getInventory().removeItem(itemName);

        std::cout << "糟糕！你被发现了！\n";
        std::cout << "压力 +50\n";
        std::cout << "MP4 被没收了。\n";
    }
}

void Action::playPhone(Player& player)
{
    const std::string itemName = "phone";

    if (!hasItem(player, itemName))
    {
        std::cout << "你没有手机。\n";
        return;
    }

    modifyStat(player, StatType::Stress, -15);

    std::cout << "你偷偷玩了一会儿手机。\n";
    std::cout << "压力 -15\n";

    if (checkCaught(15))
    {
        modifyStat(player, StatType::Stress, 100);
        player.getInventory().removeItem(itemName);

        std::cout << "糟糕！你玩手机的时候被发现了！\n";
        std::cout << "压力 +100\n";
        std::cout << "手机被没收了。\n";
    }
}

void Action::readNovel(Player& player)
{
    const std::string itemName = "novel";

    if (!hasItem(player, itemName))
    {
        std::cout << "你没有小说。\n";
        return;
    }

    modifyStat(player, StatType::Stress, -10);

    std::cout << "你偷偷看了一会儿小说。\n";
    std::cout << "压力 -10\n";

    if (checkCaught(25))
    {
        modifyStat(player, StatType::Stress, 30);
        player.getInventory().removeItem(itemName);

        std::cout << "糟糕！你看小说的时候被发现了！\n";
        std::cout << "压力 +30\n";
        std::cout << "小说被没收了。\n";
    }
}

void Action::entertain(Player& player)
{
    std::cout << "\n========== 娱乐 ==========\n";
    std::cout << "1. 吃零食（消耗零食，压力 -8）\n";
    std::cout << "2. 玩MP4（压力 -10，有 10% 概率被没收）\n";
    std::cout << "3. 玩手机（压力 -15，有 15% 概率被没收）\n";
    std::cout << "4. 看小说（压力 -10，有 25% 概率被没收）\n";

    int choice = 0;
    if (!CommandParser::readChoice(choice, 1, 4))
    {
        return;
    }

    switch (choice)
    {
    case 1: eatSnack(player); break;
    case 2: playMP4(player); break;
    case 3: playPhone(player); break;
    case 4: readNovel(player); break;
    default:
        std::cout << "无效选择。\n";
        break;
    }
}

void Action::exercise(Player& player)
{
    std::cout << "\n========== 锻炼 ==========\n";

    modifyStat(player, StatType::Stress, -8);
    modifyStat(player, StatType::Health, 5);
    modifyStat(player, StatType::Stamina, -10);

    std::cout << "你在体育馆锻炼了一会儿。\n";
    std::cout << "压力 -8，健康 +5，体力 -10\n";
}

void Action::socialize(Player& player)
{
    std::cout << "\n========== 社交 ==========\n";
    std::cout << "1. 和同学聊天（情商 +5，压力 -5，体力 -2）\n";
    std::cout << "2. 和同学讨论学习（情商 +5，智力 +2，压力 -3）\n";

    int choice = 0;
    if (!CommandParser::readChoice(choice, 1, 2))
    {
        return;
    }

    switch (choice)
    {
    case 1:
        modifyStat(player, StatType::EQ, 5);
        modifyStat(player, StatType::Stress, -5);
        modifyStat(player, StatType::Stamina, -2);
        std::cout << "你和同学聊了一会儿。\n";
        std::cout << "情商 +5，压力 -5，体力 -2\n";
        break;

    case 2:
        modifyStat(player, StatType::EQ, 5);
        modifyStat(player, StatType::Intelligence, 2);
        modifyStat(player, StatType::Stress, -3);
        std::cout << "你和同学讨论了一会儿学习。\n";
        std::cout << "情商 +5，智力 +2，压力 -3\n";
        break;

    default:
        std::cout << "无效选择。\n";
        break;
    }
}

void Action::executeClassAction(Player& player)
{
    while (true)
    {
        std::cout << "\n========== 课堂行动 ==========\n";
        std::cout << "1. 认真听课（按课表，科目 +1~3，压力 +4，体力 -3）\n";
        std::cout << "2. 上课睡觉（压力 -12，体力 +10，健康 +2）\n";
        std::cout << "3. 玩手机（需持有；压力 -15，可能被没收）\n";
        std::cout << "4. 玩MP4（需购买；压力 -10，可能被没收）\n";
        std::cout << "5. 看小说（需购买；压力 -10，可能被没收）\n";
        std::cout << "6. 吃零食（需购买并消耗；压力 -8）\n";
        std::cout << "7. 专项练习（自由选科：补基础 / 错题 / 难题）\n";
        std::cout << "本节课：" << to_string(getScheduledSubject()) << "；智力低于75时听课额外 +1。\n";

        int choice = 0;
        if (!CommandParser::readChoice(choice, 1, 7))
        {
            return;
        }

        if ((choice == 3 && !hasItem(player, "phone")) ||
            (choice == 4 && !hasItem(player, "mp4")) ||
            (choice == 5 && !hasItem(player, "novel")) ||
            (choice == 6 && !hasItem(player, "snack")))
        {
            std::cout << "你还没有这个物品，请先在晚间商店购买。本次行动未消耗。\n";
            continue;
        }

        switch (choice)
        {
        case 1: attendClass(player); break;
        case 2: sleepInClass(player); break;
        case 3: playPhone(player); break;
        case 4: playMP4(player); break;
        case 5: readNovel(player); break;
        case 6: eatSnack(player); break;
        case 7: selfStudy(player); break;
        }
        return;
    }
}

void Action::executeNoonAction(Player& player)
{
    while (true)
    {
        std::cout << "\n========== 午间行动 ==========\n";
        std::cout << "1. 睡午觉（压力 -10，体力 +15，健康 +2）\n";
        std::cout << "2. 好好吃午饭（金钱 -15，体力 +10，健康 +2）\n";
        std::cout << "3. 和同学交流（情商提升，压力降低）\n";
        if (hasItem(player, "phone"))
            std::cout << "4. 玩手机（压力 -15，可能被没收）\n";
        if (hasItem(player, "mp4"))
            std::cout << "5. 玩MP4（压力 -10，可能被没收）\n";

        int choice = 0;
        if (!CommandParser::readChoice(choice, 1, 5))
        {
            return;
        }

        if ((choice == 4 && !hasItem(player, "phone")) ||
            (choice == 5 && !hasItem(player, "mp4")))
        {
            std::cout << "该选项需要先购买对应物品，本次行动未消耗。\n";
            continue;
        }

        switch (choice)
        {
        case 1: takeNap(player); break;
        case 2:
            if (player.getMoney() < 15)
            {
                std::cout << "零钱不足，先换个安排。本次行动未消耗。\n";
                continue;
            }
            player.changeMoney(-15);
            modifyStat(player, StatType::Stamina, 10);
            modifyStat(player, StatType::Health, 2);
            std::cout << "热饭让胃和心都踏实下来。金钱 -15，体力 +10，健康 +2\n";
            break;
        case 3: socialize(player); break;
        case 4: playPhone(player); break;
        case 5: playMP4(player); break;
        }
        return;
    }
}

void Action::visitLibrary(Player& player)
{
    std::cout << "\n========== 图书馆 ==========\n";
    std::cout << "1. 专项练习（补基础 / 错题 / 难题；进入后显示代价）\n";
    std::cout << "2. 整理错题（科目 +1~3，智力 +2，压力 +5，体力 -4）\n";

    int choice = 0;
    if (!CommandParser::readChoice(choice, 1, 2))
        return;

    if (choice == 1)
    {
        selfStudy(player);
        return;
    }

    const StatType subject = chooseSubject();
    const int gain = calculateStudyGain(player, subject, 3);
    modifyStat(player, subject, gain);
    modifyStat(player, StatType::Intelligence, 2);
    modifyStat(player, StatType::Stress, 5);
    modifyStat(player, StatType::Stamina, -4);
    std::cout << "你整理了近期错题。" << to_string(subject) << " +" << gain
              << "，智力 +2，压力 +5，体力 -4\n";
}

void Action::visitGym(Player& player)
{
    std::cout << "\n========== 体育馆 ==========\n";
    std::cout << "1. 跑步锻炼（压力 -8，健康 +5，体力 -10）\n";
    std::cout << "2. 和同学打球（情商 +3，压力 -10，健康 +3，体力 -8）\n";

    int choice = 0;
    if (!CommandParser::readChoice(choice, 1, 2))
        return;

    if (choice == 1)
    {
        exercise(player);
        return;
    }

    modifyStat(player, StatType::EQ, 3);
    modifyStat(player, StatType::Stress, -10);
    modifyStat(player, StatType::Health, 3);
    modifyStat(player, StatType::Stamina, -8);
    std::cout << "你和同学打了一场球。情商 +3，压力 -10，健康 +3，体力 -8\n";
}

void Action::visitArcade(Player& player)
{
    std::cout << "\n========== 游戏厅 ==========\n";
    std::cout << "1. 玩一会街机（金钱 -30，压力 -15）\n";
    std::cout << "2. 和同学联机（金钱 -50，情商 +4，压力 -12）\n";

    int choice = 0;
    if (!CommandParser::readChoice(choice, 1, 2))
        return;

    const int cost = choice == 1 ? 30 : 50;
    if (player.getMoney() < cost)
    {
        std::cout << "你的钱不够，本次没有消费。\n";
        return;
    }

    player.changeMoney(-cost);
    modifyStat(player, StatType::Stress, choice == 1 ? -15 : -12);
    if (choice == 2)
        modifyStat(player, StatType::EQ, 4);

    std::cout << (choice == 1
        ? "你放松地玩了一会街机。金钱 -30，压力 -15\n"
        : "你和同学玩得很开心。金钱 -50，情商 +4，压力 -12\n");
}

void Action::buyItem(
    Player& player,
    const std::string& id,
    const std::string& name,
    int price)
{
    if (id != "snack" && player.getInventory().hasItem(id))
    {
        std::cout << "你已经拥有" << name << "，不需要重复购买。\n";
        return;
    }

    if (player.getMoney() < price)
    {
        std::cout << "金钱不足，无法购买" << name << "。\n";
        return;
    }

    player.changeMoney(-price);
    player.getInventory().addItem(
        std::make_unique<Item>(id, name, price));
    std::cout << "购买成功：" << name << "，金钱 -" << price << "。\n";
}

void Action::visitShop(Player& player)
{
    std::cout << "\n========== 商店 ==========\n";
    std::cout << "当前金钱：" << player.getMoney() << "\n";
    std::cout << "1. 零食（20，可重复购买，使用后压力 -8）\n";
    std::cout << "2. 小说（80，娱乐后压力 -10）\n";
    std::cout << "3. MP4（300，娱乐后压力 -10）\n";
    std::cout << "4. 手机（800，娱乐后压力 -15）\n";
    std::cout << "0. 离开商店\n";

    int choice = 0;
    if (!CommandParser::readChoice(choice, 0, 4))
        return;

    switch (choice)
    {
    case 1: buyItem(player, "snack", "零食", 20); break;
    case 2: buyItem(player, "novel", "小说", 80); break;
    case 3: buyItem(player, "mp4", "MP4", 300); break;
    case 4: buyItem(player, "phone", "手机", 800); break;
    default: std::cout << "你离开了商店。\n"; break;
    }
}

void Action::visitHome(Player& player)
{
    std::cout << "\n========== 家 ==========\n";
    std::cout << "1. 提前休息（压力 -25，体力 +20，健康 +5）\n";
    std::cout << "2. 和家人聊天（情商 +4，压力 -10）\n";

    int choice = 0;
    if (!CommandParser::readChoice(choice, 1, 2))
        return;

    if (choice == 1)
    {
        earlyRest(player);
        return;
    }

    modifyStat(player, StatType::EQ, 4);
    modifyStat(player, StatType::Stress, -10);
    std::cout << "家人的支持让你安心了许多。情商 +4，压力 -10\n";
}

void Action::executeEveningAction(Player& player)
{
    // 晚间是唯一开放自由移动的时段；固定目的地和当前位置活动共用同一入口。
    if (world) world->look();
    std::cout << "\n========== 晚间行动 ==========\n";
    std::cout << "1. 图书馆（学习与整理错题）\n";
    std::cout << "2. 体育馆（锻炼或和同学打球）\n";
    std::cout << "3. 游戏厅（花钱娱乐）\n";
    std::cout << "4. 商店（购买道具）\n";
    std::cout << "5. 家（休息或陪伴家人）\n";
    std::cout << "6. 在当前位置活动（map 看地图，方向命令移动）\n";
    std::cout << "0. 保存当前进度并退出\n";

    int choice = 0;
    if (!CommandParser::readChoice(choice, 0, 6))
    {
        return;
    }

    if (world && choice >= 1 && choice <= 5)
    {
        const char* destinations[] = {"library", "gym", "arcade", "shop", "home"};
        world->travelTo(destinations[choice - 1]);
    }
    if (choice == 6)
    {
        const std::string location = world ? world->currentRoom().id : "home";
        if (location == "library" || location == "classroom") visitLibrary(player);
        else if (location == "gym" || location == "gate") exercise(player);
        else if (location == "arcade") visitArcade(player);
        else if (location == "shop") visitShop(player);
        else if (location == "home") visitHome(player);
        else if (location == "office")
        {
            std::cout << "你带着问题整理思路，向老师请教后进行专项练习。\n";
            selfStudy(player);
        }
        else
        {
            std::cout << "1. 吃晚饭（金钱 -15，体力 +10，健康 +2）\n"
                      << "2. 在食堂歇一会（压力 -5）\n";
            int meal = 0;
            while (CommandParser::readChoice(meal, 1, 2))
            {
                if (meal == 1 && player.getMoney() < 15)
                {
                    CommandParser::cancelReplay();
                    std::cout << "零钱不足，换个安排吧。\n";
                    continue;
                }
                if (meal == 1)
                {
                    player.changeMoney(-15);
                    modifyStat(player, StatType::Stamina, 10);
                    modifyStat(player, StatType::Health, 2);
                    std::cout << "热饭下肚。金钱 -15，体力 +10，健康 +2。\n";
                }
                else
                {
                    modifyStat(player, StatType::Stress, -5);
                    std::cout << "坐在空下来的饭桌旁歇一会，压力 -5。\n";
                }
                break;
            }
        }
        return;
    }
    switch (choice)
    {
    case 0:
        exitRequested = true;
        std::cout << "正在保存当前时段，下次仍从这里继续。\n";
        break;

    case 1:
        visitLibrary(player);
        break;

    case 2:
        visitGym(player);
        break;

    case 3:
        visitArcade(player);
        break;

    case 4:
        visitShop(player);
        break;

    case 5:
        visitHome(player);
        break;

    default:
        std::cout << "无效选择。\n";
        break;
    }
}

void Action::executeDailyAction(Player& player)
{
    // 根据 GameManager 写入的时段分派菜单，结束后统一输出时段结算提示。
    ConsoleUI::showPeriod(static_cast<int>(currentTime),
        player.getStats().get(StatType::Stamina), player.getStats().get(StatType::Stress));
    switch (currentTime)
    {
    case ActionTime::Morning:
        std::cout << "\n【上午】\n";
        executeMorningAction(player);
        break;

    case ActionTime::Noon:
        std::cout << "\n【中午】\n";
        executeNoonAction(player);
        break;

    case ActionTime::Afternoon:
        std::cout << "\n【下午】\n";
        executeClassAction(player);
        break;

    case ActionTime::Evening:
        std::cout << "\n【晚上】\n";
        executeEveningAction(player);
        break;
    }

    showActionResult(player);
}

void Action::executeMorningAction(Player& player)
{
    std::cout << "1. 英语晨读（英语 +1~3，压力 +2，体力 -2）\n"
              << "2. 买份早餐（金钱 -10，体力 +8，健康 +2）\n"
              << "3. 校园慢走（压力 -6，体力 -2）\n"
              << "4. 梳理今日弱科（最弱科 +1，压力 -2）\n";
    int choice = 0;
    while (CommandParser::readChoice(choice, 1, 4))
    {
        if (choice == 2 && player.getMoney() < 10)
        {
            std::cout << "零钱不够，换个晨间安排吧。\n";
            continue;
        }
        if (choice == 1)
        {
            const int gain = calculateStudyGain(player, StatType::English, 3);
            modifyStat(player, StatType::English, gain);
            modifyStat(player, StatType::Stress, 2);
            modifyStat(player, StatType::Stamina, -2);
            std::cout << "晨读声融进晨光。英语 +" << gain << "，压力 +2，体力 -2\n";
        }
        if (choice == 2)
        {
            player.changeMoney(-10);
            modifyStat(player, StatType::Stamina, 8);
            modifyStat(player, StatType::Health, 2);
            std::cout << "豆浆还是热的。金钱 -10，体力 +8，健康 +2\n";
        }
        if (choice == 3)
        {
            modifyStat(player, StatType::Stress, -6);
            modifyStat(player, StatType::Stamina, -2);
            std::cout << "绕操场走了一圈，呼吸慢下来。压力 -6，体力 -2\n";
        }
        if (choice == 4)
        {
            const StatType subjects[] = {StatType::Chinese, StatType::Math, StatType::English, StatType::Science};
            StatType weakest = subjects[0];
            for (StatType subject : subjects)
                if (player.getStats().get(subject) < player.getStats().get(weakest)) weakest = subject;
            modifyStat(player, weakest, 1);
            modifyStat(player, StatType::Stress, -2);
            std::cout << "回顾薄弱科目的笔记。" << to_string(weakest) << " +1，压力 -2\n";
        }
        return;
    }
}

void Action::showActionResult(Player& player)
{
    (void)player;
    std::cout << "\n------------------------\n";
    std::cout << "本时段行动结束。\n";
}
