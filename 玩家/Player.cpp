
#include "Player.h"

#include <iostream>
#include <stdexcept>
#include <utility>

Player::Player(
    const std::string& name,
    std::unique_ptr<Stats> stats,
    int money
)
    : name_(name),
      stats_(std::move(stats)),
      money_(money)
{
    if (name_.empty())
    {
        throw std::invalid_argument("Player name cannot be empty");
    }

    if (!stats_)
    {
        stats_ = std::make_unique<Stats>();
    }

    if (money_ < 0)
    {
        money_ = 0;
    }
}

const std::string& Player::getName() const
{
    return name_;
}

const Stats& Player::getStats() const
{
    return *stats_;
}

Stats& Player::getStats()
{
    return *stats_;
}

int Player::getMoney() const
{
    return money_;
}

void Player::changeMoney(int change)
{
    money_ += change;

    if (money_ < 0)
    {
        money_ = 0;
    }
}

void Player::modifyStat(StatType type, int change)
{
    stats_->modify(type, change);
}

void Player::showStatus() const
{
    std::cout << "===== Player Status =====" << '\n';
    std::cout << "Name:  " << name_ << '\n';
    std::cout << "Money: " << money_ << '\n';

    stats_->show();

    std::cout << "=========================" << '\n';
}
