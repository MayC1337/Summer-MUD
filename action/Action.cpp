#include "Action.h"
#include <random>

namespace {
int randomPercent() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(1, 100);
    return dist(gen);
}

void modifySubject(Stats& stats, StudySubject subject, int value) {
    switch (subject) {
    case StudySubject::Chinese: stats.modify(StatType::Chinese, value); break;
    case StudySubject::Math: stats.modify(StatType::Math, value); break;
    case StudySubject::English: stats.modify(StatType::English, value); break;
    case StudySubject::Physics: stats.modify(StatType::Physics, value); break;
    case StudySubject::Chemistry: stats.modify(StatType::Chemistry, value); break;
    case StudySubject::Biology: stats.modify(StatType::Biology, value); break;
    }
}

bool tryConsume(Player& player, ItemType type, const char* missingMessage) {
    if (!player.hasItem(type)) {
        std::cout << missingMessage << "\n";
        return false;
    }
    return player.removeItem(type);
}
}

void Action::applyStress(Player& player, int delta) {
    player.getStats().modify(StatType::Stress, delta);
}

void Action::applyHealth(Player& player, int delta) {
    player.getStats().modify(StatType::Health, delta);
}

int Action::scaledLearningGain(const Player& player, int baseGain) {
    return static_cast<int>(baseGain * player.getStats().getLearningMultiplier());
}

bool Action::study(Player& player, StudySubject subject) {
    if (player.getActionPoints() <= 0) {
        std::cout << "行动点不足。\n";
        return false;
    }

    const int subjectGain = scaledLearningGain(player, 3);
    const int intelligenceGain = scaledLearningGain(player, 1);

    modifySubject(player.getStats(), subject, subjectGain);
    player.getStats().modify(StatType::Intelligence, intelligenceGain);
    player.getStats().modify(StatType::Stress, 5);
    player.getStats().modify(StatType::Health, -1);
    player.consumeActionPoint();

    std::cout << "认真听课完成：学科+" << subjectGain
              << "，智商+" << intelligenceGain
              << "，压力+5，健康-1。\n";
    return true;
}

bool Action::rest(Player& player) {
    if (player.getActionPoints() <= 0) return false;
    player.getStats().modify(StatType::Stress, -10);
    player.getStats().modify(StatType::Health, 3);
    player.consumeActionPoint();
    return true;
}

bool Action::entertain(Player& player) {
    if (player.getActionPoints() <= 0) return false;
    player.getStats().modify(StatType::Stress, -15);
    player.consumeActionPoint();
    return true;
}

bool Action::socialize(Player& player) {
    if (player.getActionPoints() <= 0) return false;
    player.getStats().modify(StatType::EQ, 3);
    player.getStats().modify(StatType::Stress, -5);
    player.consumeActionPoint();
    return true;
}

bool StudyAction::executeDailyAction(Player& player) {
    if (player.getActionPoints() <= 0) {
        std::cout << "行动点不足。\n";
        return false;
    }

    const int baseSubject = selfStudy_ ? 5 : 3;
    const int subjectGain = scaledLearningGain(player, baseSubject);
    const int intelligenceGain = scaledLearningGain(player, 1);
    modifySubject(player.getStats(), subject_, subjectGain);
    player.getStats().modify(StatType::Intelligence, intelligenceGain);
    player.getStats().modify(StatType::Stress, selfStudy_ ? 8 : 5);
    player.getStats().modify(StatType::Health, selfStudy_ ? -2 : -1);
    player.consumeActionPoint();

    std::cout << getName() << "完成：学科+" << subjectGain
              << "，智商+" << intelligenceGain
              << "，压力+" << (selfStudy_ ? 8 : 5)
              << "，健康" << (selfStudy_ ? -2 : -1) << "。\n";
    return true;
}

bool RestAction::executeDailyAction(Player& player) {
    if (player.getActionPoints() <= 0) return false;
    player.getStats().modify(StatType::Stress, -10);
    player.getStats().modify(StatType::Health, 3);
    player.consumeActionPoint();
    std::cout << "午睡完成：压力-10，健康+3。\n";
    return true;
}

std::string EntertainmentAction::getName() const {
    switch (mode_) {
    case Mode::Phone: return "玩手机";
    case Mode::MP4: return "玩MP4";
    case Mode::Novel: return "看小说";
    case Mode::Snack: return "吃零食";
    case Mode::GameHall: return "游戏厅娱乐";
    }
    return "娱乐";
}

bool EntertainmentAction::executeDailyAction(Player& player) {
    if (player.getActionPoints() <= 0) {
        std::cout << "行动点不足。\n";
        return false;
    }

    switch (mode_) {
    case Mode::Phone: {
        if (!player.hasItem(ItemType::Phone)) {
            std::cout << "你没有手机。\n";
            return false;
        }
        // 先判定是否被发现；未被发现才获得正常减压收益。
        if (randomPercent() <= 15) {
            player.getStats().modify(StatType::Stress, 100);
            player.removeItem(ItemType::Phone);
            player.consumeActionPoint();
            std::cout << "糟糕！你玩手机被发现了，压力+100，手机被没收。\n";
        } else {
            player.getStats().modify(StatType::Stress, -18);
            player.consumeActionPoint();
            std::cout << "玩手机成功：压力-18。\n";
        }
        return true;
    }
    case Mode::MP4: {
        if (!player.hasItem(ItemType::MP4)) {
            std::cout << "你没有MP4。\n";
            return false;
        }
        if (randomPercent() <= 10) {
            player.getStats().modify(StatType::Stress, 50);
            player.removeItem(ItemType::MP4);
            player.consumeActionPoint();
            std::cout << "糟糕！你玩MP4被发现了，压力+50，MP4被没收。\n";
        } else {
            player.getStats().modify(StatType::Stress, -12);
            player.consumeActionPoint();
            std::cout << "玩MP4成功：压力-12。\n";
        }
        return true;
    }
    case Mode::Novel: {
        if (!player.hasItem(ItemType::Novel)) {
            std::cout << "你没有小说。\n";
            return false;
        }
        if (randomPercent() <= 25) {
            player.getStats().modify(StatType::Stress, 30);
            player.removeItem(ItemType::Novel);
            player.consumeActionPoint();
            std::cout << "糟糕！你看小说被发现了，压力+30，小说被没收。\n";
        } else {
            player.getStats().modify(StatType::Stress, -10);
            player.consumeActionPoint();
            std::cout << "看小说成功：压力-10。\n";
        }
        return true;
    }
    case Mode::Snack: {
        if (!tryConsume(player, ItemType::Snack, "你没有零食。")) return false;
        player.getStats().modify(StatType::Stress, -8);
        player.getStats().modify(StatType::Health, -1);
        player.consumeActionPoint();
        std::cout << "吃零食：压力-8，健康-1，消耗1份零食。\n";
        return true;
    }
    case Mode::GameHall:
        if (player.getMoney() < 20) {
            std::cout << "钱不够去游戏厅。\n";
            return false;
        }
        player.changeMoney(-20);
        player.getStats().modify(StatType::Stress, -15);
        player.consumeActionPoint();
        std::cout << "游戏厅娱乐：压力-15，金钱-20。\n";
        return true;
    }
    return false;
}

std::string SocializeAction::getName() const {
    switch (mode_) {
    case Mode::Chat: return "普通聊天";
    case Mode::DiscussStudy: return "讨论学习";
    case Mode::HelpFriend: return "帮同学讲题";
    }
    return "社交";
}

bool SocializeAction::executeDailyAction(Player& player) {
    if (player.getActionPoints() <= 0) return false;

    switch (mode_) {
    case Mode::Chat:
        player.getStats().modify(StatType::EQ, 3);
        player.getStats().modify(StatType::Stress, -5);
        break;
    case Mode::DiscussStudy:
        modifySubject(player.getStats(), subject_, 2);
        player.getStats().modify(StatType::EQ, 1);
        player.getStats().modify(StatType::Stress, -3);
        break;
    case Mode::HelpFriend:
        modifySubject(player.getStats(), subject_, 2);
        player.getStats().modify(StatType::EQ, 3);
        player.getStats().modify(StatType::Stress, -2);
        break;
    }

    player.consumeActionPoint();
    std::cout << getName() << "完成。\n";
    return true;
}

bool ExerciseAction::executeDailyAction(Player& player) {
    if (player.getActionPoints() <= 0) return false;
    player.getStats().modify(StatType::Stress, -10);
    player.getStats().modify(StatType::Health, 2);
    player.consumeActionPoint();
    std::cout << "体育锻炼完成：压力-10，健康+2。\n";
    return true;
}
