
#ifndef SUMMER_MUD_PLAYER_H
#define SUMMER_MUD_PLAYER_H

#include "Stats.h"

#include <memory>
#include <string>

class Player
{
public:
    Player(
        const std::string& name,
        std::unique_ptr<Stats> stats,
        int money = 0
    );

    const std::string& getName() const;

    const Stats& getStats() const;
    Stats& getStats();

    int getMoney() const;
    void changeMoney(int change);

    void modifyStat(StatType type, int change);
    void showStatus() const;

private:
    std::string name_;
    std::unique_ptr<Stats> stats_;
    int money_;
};

#endif
