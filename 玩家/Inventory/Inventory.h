
#ifndef INVENTORY_H
#define INVENTORY_H

#include "Item.h"
#include <vector>
#include <memory>
#include <string>

class Inventory
{
public:
    void addItem(std::unique_ptr<Item> item);
    bool removeItem(const std::string& id);
    bool hasItem(const std::string& id) const;
    const std::vector<std::unique_ptr<Item>>& getItems() const;
    void showItems() const;

private:
    std::vector<std::unique_ptr<Item>> items_;
};

#endif
