#include "Item.h"
#include <algorithm>
#include <utility>

Item::Item(std::string name, ItemType type, int price, int quantity)
    : name_(std::move(name)), type_(type), price_(price), quantity_(std::max(0, quantity)) {}

void Item::add(int amount) {
    if (amount > 0) quantity_ += amount;
}

bool Item::consume(int amount) {
    if (amount <= 0 || quantity_ < amount) return false;
    quantity_ -= amount;
    return true;
}
