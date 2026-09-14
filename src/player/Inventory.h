
#ifndef INVENTORY_H
#define INVENTORY_H

#include "Item.h"
#include <vector>
#include <memory>
#include <string>

// 玩家背包拥有 Item 对象，并按道具 ID 提供查询与移除操作。
class Inventory
{
public:
    void addItem(std::unique_ptr<Item> item);
    void clear();
    bool removeItem(const std::string& id);
    bool hasItem(const std::string& id) const;
    const std::vector<std::unique_ptr<Item>>& getItems() const;
    void showItems() const;

private:
    std::vector<std::unique_ptr<Item>> items_; // 背包对道具拥有唯一所有权。
};

#endif
