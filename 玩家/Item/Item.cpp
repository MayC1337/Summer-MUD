
#include "Item.h"
#include "Player.h"
#include <iostream>

Item::Item(std::string id, std::string type, int price, std::map<StatType, int> effects)
    : id_(std::move(id)),
      type_(std::move(type)),
      price_(price),
      effects_(std::move(effects))
{
}

const std::string& Item::getId() const { return id_; }
const std::string& Item::getType() const { return type_; }
int Item::getPrice() const { return price_; }
const std::map<StatType, int>& Item::getEffects() const { return effects_; }

void Item::use(Player& player) const
{
    for (const auto& [stat, val] : effects_)
        player.modifyStat(stat, val);
}

void Item::show() const
{
    std::cout << "  [" << id_ << "] " << type_
              << "  价格: " << price_;
    if (!effects_.empty())
    {
        std::cout << "  效果: ";
        for (const auto& [stat, val] : effects_)
            std::cout << to_string(stat) << (val >= 0 ? "+" : "") << val << " ";
    }
    std::cout << '\n';
}
