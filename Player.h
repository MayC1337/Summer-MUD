#pragma once
#include <array>
#include <string>

enum class StatType {
    Chinese,
    Math,
    English,
    Science,
    intelligence,
    Health,
    Stress
};

class Stats {
public:
    int values[7] = {0, 0, 0, 0, 0, 0, 0};
    int health = 100;

    int get(StatType type) const {
        return values[static_cast<int>(type)];
    }

    void set(StatType type, int value) {
        values[static_cast<int>(type)] = value;
    }
};

class Player {
public:
    Stats& getStats() {
        return stats;
    }

    const Stats& getStats() const {
        return stats;
    }

    Stats stats;
};

