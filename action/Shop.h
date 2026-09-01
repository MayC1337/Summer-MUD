#pragma once

#include "Item.h"
#include "Player.h"
#include <vector>

class Shop {
private:
    std::vector<Item> goods_;

public:
    Shop();

    const std::vector<Item>& getGoods() const { return goods_; }
    void showGoods() const;
    bool buy(Player& player, ItemType type, int quantity = 1) const;
};
