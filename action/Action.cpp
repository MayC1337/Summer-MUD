#include "Action.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

Action::Action()
    : currentTime(ActionTime::Morning)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void Action::setTime(ActionTime time)
{
    currentTime = time;
}

ActionTime Action::getTime() const
{
    return currentTime;
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
    return (std::rand() % 100) < probability;
}

double Action::getStudyMultiplier(Player& player)
{
    Stats& stats = player.getStats();

    int stress = stats.get(StatType::Stress);
    int health = stats.get(StatType::Health);

    if (stress >= 200)
        return 0.0;

    double multiplier = 1.0;

    if (stress >= 100)
        multiplier *= 0.5;

    if (health < 15)
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

    int choice;
    std::cin >> choice;

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
    modifyStat(player, StatType::Stress, 10);

    std::cout << "你认真听了一节课。\n";
    std::cout << "对应科目熟练度 +" << gain << "\n";
    std::cout << "压力 +10\n";

    if (multiplier < 1.0)
        std::cout << "由于你的状态不佳，学习效率下降了。\n";
}

void Action::sleepInClass(Player& player)
{
    std::cout << "\n========== 上课睡觉 ==========\n";

    modifyStat(player, StatType::Stress, -15);
    modifyStat(player, StatType::Health, 1);

    std::cout << "你趁老师不注意睡了一会儿。\n";
    std::cout << "压力 -15\n";
    std::cout << "健康 +1\n";
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
    modifyStat(player, StatType::Stress, 10);

    std::cout << "你完成了一次自习。\n";
    std::cout << "对应科目熟练度 +" << gain << "\n";
    std::cout << "压力 +10\n";
}

void Action::takeNap(Player& player)
{
    std::cout << "\n========== 睡午觉 ==========\n";

    modifyStat(player, StatType::Stress, -10);

    std::cout << "你睡了一觉午觉。\n";
    std::cout << "压力 -10\n";
}

void Action::earlyRest(Player& player)
{
    std::cout << "\n========== 提前休息 ==========\n";

    modifyStat(player, StatType::Stress, -25);
    modifyStat(player, StatType::Health, 1);

    std::cout << "你决定今晚早点睡觉。\n";
    std::cout << "压力 -25\n";
    std::cout << "健康 +1\n";
}

void Action::study(Player& player)
{
    if (currentTime == ActionTime::Morning ||
        currentTime == ActionTime::Afternoon)
    {
        std::cout << "\n========== 学习 ==========\n";
        std::cout << "1. 认真听课\n";
        std::cout << "2. 自习\n";

        int choice;
        std::cin >> choice;

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
    std::cout << "1. 吃零食\n";
    std::cout << "2. 玩MP4\n";
    std::cout << "3. 玩手机\n";
    std::cout << "4. 看小说\n";

    int choice;
    std::cin >> choice;

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

    modifyStat(player, StatType::Stress, -10);
    modifyStat(player, StatType::Health, 2);

    std::cout << "你在体育馆锻炼了一会儿。\n";
    std::cout << "压力 -10\n";
    std::cout << "健康 +2\n";
}

void Action::socialize(Player& player)
{
    std::cout << "\n========== 社交 ==========\n";
    std::cout << "1. 和同学聊天\n";
    std::cout << "2. 和同学讨论学习\n";

    int choice;
    std::cin >> choice;

    switch (choice)
    {
    case 1:
        modifyStat(player, StatType::EQ, 5);
        modifyStat(player, StatType::Stress, -5);
        std::cout << "你和同学聊了一会儿。\n";
        std::cout << "情商 +5，压力 -5\n";
        break;

    case 2:
        modifyStat(player, StatType::EQ, 5);
        modifyStat(player, StatType::Stress, -3);
        std::cout << "你和同学讨论了一会儿学习。\n";
        std::cout << "情商 +5，压力 -3\n";
        break;

    default:
        std::cout << "无效选择。\n";
        break;
    }
}

void Action::executeClassAction(Player& player)
{
    std::cout << "\n========== 课堂行动 ==========\n";
    std::cout << "1. 认真听课\n";
    std::cout << "2. 上课睡觉\n";
    std::cout << "3. 玩手机\n";
    std::cout << "4. 玩MP4\n";
    std::cout << "5. 看小说\n";
    std::cout << "6. 吃零食\n";
    std::cout << "7. 自习\n";

    int choice;
    std::cin >> choice;

    switch (choice)
    {
    case 1: attendClass(player); break;
    case 2: sleepInClass(player); break;
    case 3: playPhone(player); break;
    case 4: playMP4(player); break;
    case 5: readNovel(player); break;
    case 6: eatSnack(player); break;
    case 7: selfStudy(player); break;
    default:
        std::cout << "无效选择。\n";
        break;
    }
}

void Action::executeNoonAction(Player& player)
{
    std::cout << "\n========== 午间行动 ==========\n";
    std::cout << "1. 睡午觉\n";
    std::cout << "2. 自习\n";
    std::cout << "3. 玩手机\n";
    std::cout << "4. 玩MP4\n";

    int choice;
    std::cin >> choice;

    switch (choice)
    {
    case 1: takeNap(player); break;
    case 2: selfStudy(player); break;
    case 3: playPhone(player); break;
    case 4: playMP4(player); break;
    default:
        std::cout << "无效选择。\n";
        break;
    }
}

void Action::executeEveningAction(Player& player)
{
    std::cout << "\n========== 晚间行动 ==========\n";
    std::cout << "1. 图书馆\n";
    std::cout << "2. 体育馆\n";
    std::cout << "3. 游戏厅\n";
    std::cout << "4. 商店\n";
    std::cout << "5. 家\n";

    int choice;
    std::cin >> choice;

    switch (choice)
    {
    case 1:
        study(player);
        break;

    case 2:
        exercise(player);
        break;

    case 3:
        entertain(player);
        break;

    case 4:
        std::cout << "进入商店。\n";
        std::cout << "购物功能由 Shop 模块负责。\n";
        break;

    case 5:
        earlyRest(player);
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
    std::cout << "\n------------------------\n";
    std::cout << "行动结束，当前状态：\n";

    player.showStatus();

    std::cout << "------------------------\n";
}
