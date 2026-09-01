#ifndef PLAYER_H
#define PLAYER_H

#include "Stats.h"
#include <string>
#include <memory>

class Inventory;

class Player
{
public:
    Player(const std::string& name,
        std::unique_ptr<Stats> stats = nullptr,
        int money = 0);

    const std::string& getName() const;
    int getMoney() const;
    void changeMoney(int change);

    Stats& getStats();
    const Stats& getStats() const;
    void modifyStat(StatType type, int change);

    Inventory& getInventory();
    const Inventory& getInventory() const;

    void showStatus() const;

private:
    std::string name_;
    std::unique_ptr<Stats> stats_;
    int money_;
    std::unique_ptr<Inventory> inventory_;
};

#endif
