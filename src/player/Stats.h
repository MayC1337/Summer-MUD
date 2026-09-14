#ifndef STATS_H
#define STATS_H

#include <string>

enum class StatType
{
    Intelligence,
    EQ,
    Stamina,
    Health,
    Stress,
    Chinese,
    Math,
    English,
    Science
};

std::string to_string(StatType type);

class Stats
{
public:
    Stats(int intelligence = 40,
        int eq = 40,
        int stamina = 80,
        int health = 80,
        int stress = 10,
        int chinese = 45,
        int math = 45,
        int english = 45,
        int science = 45);

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
    int chinese_;
    int math_;
    int english_;
    int science_;
};

#endif
