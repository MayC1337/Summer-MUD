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

class Action
{
private:
    ActionTime currentTime;
    int currentDayOfWeek;
    bool exitRequested;
    CampusMap* world = nullptr;

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
