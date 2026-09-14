#ifndef PLAYER_H
#define PLAYER_H

#include "Stats.h"
#include <string>
#include <memory>

class Inventory;

// 聚合玩家姓名、金钱、属性和背包，并作为玩法模块的唯一玩家入口。
class Player
{
public:
    Player(const std::string& name,
        std::unique_ptr<Stats> stats = nullptr,
        int money = 2000);
    ~Player();

    const std::string& getName() const;
    void setName(const std::string& name);
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
    std::unique_ptr<Stats> stats_; // 允许读档时替换整组属性。
    int money_;
    std::unique_ptr<Inventory> inventory_;
};

#endif
