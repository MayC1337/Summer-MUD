#include "Action.h"
#include "../command/CommandParser.h"
#include "../player/Inventory.h"
#include "../player/Item.h"
#include "../player/Player.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <random>

Action::Action()
    : currentTime(ActionTime::Morning), exitRequested(false)
{
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

StatType Action::chooseSubject()
{
    std::cout << "\n请选择学习科目：\n";
    std::cout << "1. 语文\n";
    std::cout << "2. 数学\n";
    std::cout << "3. 英语\n";
    std::cout << "4. 科学\n";

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

    StatType subject = chooseSubject();
    double multiplier = getStudyMultiplier(player);
    int gain = static_cast<int>(5 * multiplier);

    if (gain < 0)
        gain = 0;

    modifyStat(player, subject, gain);
    modifyStat(player, StatType::Stress, 6);
    modifyStat(player, StatType::Stamina, -3);

    std::cout << "你认真听了一节课。\n";
    std::cout << "对应科目熟练度 +" << gain << "\n";
    std::cout << "压力 +6，体力 -3\n";

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
    std::cout << "\n========== 自习 ==========\n";

    StatType subject = chooseSubject();
    double multiplier = getStudyMultiplier(player);
    int gain = static_cast<int>(5 * multiplier);

    if (gain < 0)
        gain = 0;

    modifyStat(player, subject, gain);
    modifyStat(player, StatType::Stress, 8);
    modifyStat(player, StatType::Stamina, -5);

    std::cout << "你完成了一次自习。\n";
    std::cout << "对应科目熟练度 +" << gain << "\n";
    std::cout << "压力 +8，体力 -5\n";
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
        std::cout << "1. 认真听课（科目基础 +5，压力 +6，体力 -3）\n";
        std::cout << "2. 自习（科目基础 +5，压力 +8，体力 -5）\n";

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
        std::cout << "1. 认真听课（科目基础 +5，压力 +6，体力 -3）\n";
        std::cout << "2. 上课睡觉（压力 -12，体力 +10，健康 +2）\n";
        std::cout << "3. 玩手机（需持有；压力 -15，可能被没收）\n";
        std::cout << "4. 玩MP4（需购买；压力 -10，可能被没收）\n";
        std::cout << "5. 看小说（需购买；压力 -10，可能被没收）\n";
        std::cout << "6. 吃零食（需购买并消耗；压力 -8）\n";
        std::cout << "7. 自习（科目基础 +5，压力 +8，体力 -5）\n";

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
        std::cout << "2. 自习（科目基础 +5，压力 +8，体力 -5）\n";
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
        case 2: selfStudy(player); break;
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
    std::cout << "1. 专心自习（科目基础 +5，压力 +8，体力 -5）\n";
    std::cout << "2. 整理错题（科目 +3，智力 +2，压力 +5，体力 -4）\n";

    int choice = 0;
    if (!CommandParser::readChoice(choice, 1, 2))
        return;

    if (choice == 1)
    {
        selfStudy(player);
        return;
    }

    const StatType subject = chooseSubject();
    modifyStat(player, subject, 3);
    modifyStat(player, StatType::Intelligence, 2);
    modifyStat(player, StatType::Stress, 5);
    modifyStat(player, StatType::Stamina, -4);
    std::cout << "你整理了近期错题。对应科目 +3，智力 +2，压力 +5，体力 -4\n";
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
    std::cout << "\n========== 晚间行动 ==========\n";
    std::cout << "1. 图书馆（学习与整理错题）\n";
    std::cout << "2. 体育馆（锻炼或和同学打球）\n";
    std::cout << "3. 游戏厅（花钱娱乐）\n";
    std::cout << "4. 商店（购买道具）\n";
    std::cout << "5. 家（休息或陪伴家人）\n";
    std::cout << "0. 保存当前进度并退出\n";

    int choice = 0;
    if (!CommandParser::readChoice(choice, 0, 5))
    {
        return;
    }

    switch (choice)
    {
    case 0:
        exitRequested = true;
        std::cout << "今天的行动到此结束，正在保存进度。\n";
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
    switch (currentTime)
    {
    case ActionTime::Morning:
        std::cout << "\n【上午】\n";
        executeClassAction(player);
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

void Action::showActionResult(Player& player)
{
    (void)player;
    std::cout << "\n------------------------\n";
    std::cout << "本时段行动结束。\n";
}
