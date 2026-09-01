
#ifndef ITEM_H
#define ITEM_H

#include "Stats.h"
#include <map>
#include <string>

class Player;

class Item
{
public:
    Item(std::string id,
         std::string type,
         int price,
         std::map<StatType, int> effects = {});

    const std::string& getId() const;
    const std::string& getType() const;
    int getPrice() const;
    const std::map<StatType, int>& getEffects() const;

    void use(Player& player) const;
    void show() const;

private:
    std::string id_;
    std::string type_;
    int price_;
    std::map<StatType, int> effects_;
};

#endif
