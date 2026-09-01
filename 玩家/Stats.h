#ifndef STATS_H
#define STATS_H

#include <string>

enum class StatType
{
    Intelligence,
    EQ,
    Stamina,
    Health,
    Stress
};

std::string to_string(StatType type);

class Stats
{
public:
    Stats(int intelligence = 0,
        int eq = 0,
        int stamina = 0,
        int health = 0,
        int stress = 0);

    void modify(StatType type, int change);
    void set(StatType type, int value);
    int get(StatType type) const;
    void show() const;

private:
    int clamp(int value) const;

    int intelligence_;
    int eq_;
    int stamina_;
    int health_;
    int stress_;
};

#endif
