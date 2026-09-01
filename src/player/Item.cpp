
#include "Item.h"
#include "Player.h"
#include "Stats.h"
#include <iostream>

Item::Item(std::string id,
           std::string type,
           int price,
           std::vector<std::pair<StatType, int>> effects)
    : id_(std::move(id)),
      type_(std::move(type)),
      price_(price),
      effects_(std::move(effects))
{
}

const std::string& Item::getId() const { return id_; }
const std::string& Item::getType() const { return type_; }
int Item::getPrice() const { return price_; }
const std::vector<std::pair<StatType, int>>& Item::getEffects() const { return effects_; }

void Item::use(Player& player) const
{
    for (const auto& [statType, delta] : effects_)
    {
        player.modifyStat(statType, delta);
    }
}

void Item::show() const
{
    std::cout << "  [" << id_ << "] " << type_
              << "  Price: " << price_;
    if (!effects_.empty())
    {
        std::cout << "  Effects: ";
        for (const auto& [statType, delta] : effects_)
        {
            std::cout << to_string(statType) << (delta >= 0 ? "+" : "") << delta << " ";
        }
    }
    std::cout << '\n';
}
