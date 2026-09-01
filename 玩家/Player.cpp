#include "Player.h"
#include "Inventory.h"
#include <iostream>
#include <stdexcept>

Player::Player(const std::string& name, std::unique_ptr<Stats> stats, int money)
    : name_(name),
    stats_(stats ? std::move(stats) : std::make_unique<Stats>()),
    money_(money < 0 ? 0 : money),
    inventory_(std::make_unique<Inventory>())
{
    if (name_.empty())
        throw std::invalid_argument("Player name cannot be empty");
}

const std::string& Player::getName() const { return name_; }
int Player::getMoney() const { return money_; }

void Player::changeMoney(int change)
{
    money_ += change;
    if (money_ < 0) money_ = 0;
}

Stats& Player::getStats() { return *stats_; }
const Stats& Player::getStats() const { return *stats_; }

void Player::modifyStat(StatType type, int change)
{
    stats_->modify(type, change);
}

Inventory& Player::getInventory() { return *inventory_; }
const Inventory& Player::getInventory() const { return *inventory_; }

void Player::showStatus() const
{
    std::cout << "===== 玩家状态 =====\n";
    std::cout << "姓名: " << name_ << '\n';
    std::cout << "金钱: " << money_ << '\n';
    stats_->show();
    inventory_->showItems();
    std::cout << "=====================\n";
}
