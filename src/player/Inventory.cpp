
#include "Inventory.h"
#include "../core/ConsoleUI.h"
#include <algorithm>
#include <iostream>
#include <sstream>

void Inventory::addItem(std::unique_ptr<Item> item)
{
    if (!item)
    {
        return;
    }
    // 背包接管物品所有权，物品会随背包自动释放。
    items_.push_back(std::move(item));
}

void Inventory::clear()
{
    items_.clear();
}

bool Inventory::removeItem(const std::string& id)
{
    auto it = std::find_if(items_.begin(), items_.end(),
        [&](const auto& i) { return i->getId() == id; });
    if (it != items_.end()) { items_.erase(it); return true; }
    return false;
}

bool Inventory::hasItem(const std::string& id) const
{
    return std::any_of(items_.begin(), items_.end(),
        [&](const auto& i) { return i->getId() == id; });
}

const std::vector<std::unique_ptr<Item>>& Inventory::getItems() const
{
    return items_;
}

void Inventory::showItems() const
{
    ConsoleUI::boxDivider("背包");
    if (items_.empty())
    {
        ConsoleUI::boxLine("空");
        return;
    }

    for (const auto& item : items_)
    {
        std::ostringstream line;
        line << item->getType() << "  价格：" << item->getPrice();
        ConsoleUI::boxLine(line.str());
    }
}
