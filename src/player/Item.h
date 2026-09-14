
#ifndef ITEM_H
#define ITEM_H

#include "Stats.h"
#include <string>
#include <vector>
#include <utility>

class Player;

// 可购买或使用的道具；effects_ 描述其对属性的通用影响。
class Item
{
public:
    Item(std::string id, std::string type, int price,
        std::vector<std::pair<StatType, int>> effects = {});

    const std::string& getId() const;
    const std::string& getType() const;
    int getPrice() const;
    const std::vector<std::pair<StatType, int>>& getEffects() const;

    void use(Player& player) const;
    void show() const;

private:
    std::string id_;
    std::string type_;
    int price_;
    std::vector<std::pair<StatType, int>> effects_;
};

#endif
