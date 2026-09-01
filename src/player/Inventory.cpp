
#include "Inventory.h"
#include <algorithm>
#include <iostream>

void Inventory::addItem(std::unique_ptr<Item> item)
{
    items_.push_back(std::move(item));
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
    if (items_.empty()) { std::cout << "  (背包为空)\n"; return; }
    std::cout << "----- 背包 -----\n";
    for (const auto& item : items_) item->show();
}
