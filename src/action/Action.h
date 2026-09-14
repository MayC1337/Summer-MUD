#pragma once

#include "../player/Stats.h"
#include <string>

class Player;
class CampusMap;

enum class ActionTime
{
    Morning,
    Noon,
    Afternoon,
    Evening
};

// 管理一个时段内的玩家选择，并把选择转换为属性、道具与地图变化。
class Action
{
private:
    ActionTime currentTime;       // 由 GameManager 在进入时段前设置。
    int currentDayOfWeek;
    bool exitRequested;           // 晚间菜单选择退出时置位，供调度器保存并中止当天流程。
    CampusMap* world = nullptr;   // 非拥有指针；地图由 GameManager 生命周期管理。

public:
    Action();

    void setTime(ActionTime time);
    void setDayOfWeek(int dayOfWeek);
    ActionTime getTime() const;
    bool isExitRequested() const;
    void clearExitRequest();

    void study(Player& player);
    void rest(Player& player);
    void entertain(Player& player);
    void socialize(Player& player);

    void executeDailyAction(Player& player);
    void setWorld(CampusMap* map);

private:
    void executeClassAction(Player& player);
    void executeMorningAction(Player& player);
    void executeNoonAction(Player& player);
    void executeEveningAction(Player& player);

    void attendClass(Player& player);
    void sleepInClass(Player& player);
    void selfStudy(Player& player);
    void takeNap(Player& player);
    void exercise(Player& player);
    void earlyRest(Player& player);

    void visitLibrary(Player& player);
    void visitGym(Player& player);
    void visitArcade(Player& player);
    void visitShop(Player& player);
    void visitHome(Player& player);
    void buyItem(Player& player, const std::string& id,
        const std::string& name, int price);

    void eatSnack(Player& player);
    void playMP4(Player& player);
    void playPhone(Player& player);
    void readNovel(Player& player);

    StatType chooseSubject();
    StatType getScheduledSubject() const;
    int calculateStudyGain(Player& player, StatType subject, int baseGain);
    double getStudyMultiplier(Player& player);
    void modifyStat(Player& player, StatType type, int delta);

    bool hasItem(Player& player, const std::string& itemName);
    bool checkCaught(int probability);

    void showActionResult(Player& player);
};
