#include "Shop.h"
#include <iostream>

Shop::Shop() {
    goods_.emplace_back("薯片", ItemType::Snack, 50);
    goods_.emplace_back("手机", ItemType::Phone, 1500);
    goods_.emplace_back("MP4", ItemType::MP4, 500);
    goods_.emplace_back("小说", ItemType::Novel, 100);
    goods_.emplace_back("五年高考三年模拟", ItemType::StudyBook, 150);
    goods_.emplace_back("网课", ItemType::OnlineCourse, 500);
}

void Shop::showGoods() const {
    std::cout << "===== 商店 =====\n";
    for (const auto& item : goods_) {
        std::cout << item.getName() << " - " << item.getPrice() << "元\n";
    }
}

bool Shop::buy(Player& player, ItemType type, int quantity) const {
    if (quantity <= 0) return false;

    for (const auto& good : goods_) {
        if (good.getType() != type) continue;

        const long long totalCost = static_cast<long long>(good.getPrice()) * quantity;
        if (player.getMoney() < totalCost) {
            std::cout << "金钱不足。\n";
            return false;
        }

        player.changeMoney(-static_cast<int>(totalCost));
        player.addItem(Item(good.getName(), good.getType(), good.getPrice(), quantity));
        std::cout << "购买成功：" << good.getName() << " x" << quantity << "。\n";
        return true;
    }

    std::cout << "商品不存在。\n";
    return false;
}
