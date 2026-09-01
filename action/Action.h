#pragma once

#include "Player.h"
#include <string>

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

public:
    Action();

    void setTime(ActionTime time);
    ActionTime getTime() const;

    void study(Player& player);
    void rest(Player& player);
    void entertain(Player& player);
    void socialize(Player& player);

    void executeDailyAction(Player& player);

private:
    void executeClassAction(Player& player);
    void executeNoonAction(Player& player);
    void executeEveningAction(Player& player);

    void attendClass(Player& player);
    void sleepInClass(Player& player);
    void selfStudy(Player& player);
    void takeNap(Player& player);
    void exercise(Player& player);
    void earlyRest(Player& player);

    void eatSnack(Player& player);
    void playMP4(Player& player);
    void playPhone(Player& player);
    void readNovel(Player& player);

    StatType chooseSubject();
    double getStudyMultiplier(Player& player);
    void modifyStat(Player& player, StatType type, int delta);

    bool hasItem(Player& player, const std::string& itemName);
    bool checkCaught(int probability);

    void showActionResult(Player& player);
};
