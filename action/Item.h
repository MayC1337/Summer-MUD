#pragma once

#include <string>

enum class ItemType {
    Phone,
    MP4,
    Novel,
    Snack,
    StudyBook,
    OnlineCourse,
    Special
};

class Item {
private:
    std::string name_;
    ItemType type_;
    int price_ = 0;
    int quantity_ = 0;

public:
    Item(std::string name, ItemType type, int price, int quantity = 0);

    const std::string& getName() const { return name_; }
    ItemType getType() const { return type_; }
    int getPrice() const { return price_; }
    int getQuantity() const { return quantity_; }

    void add(int amount = 1);
    bool consume(int amount = 1);
};
