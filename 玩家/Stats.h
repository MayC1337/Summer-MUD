
#ifndef SUMMER_MUD_STATS_H
#define SUMMER_MUD_STATS_H

#include <iosfwd>

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

class Stats
{
public:
    Stats(
        int intelligence = 0,
        int eq = 0,
        int stamina = 0,
        int health = 0,
        int stress = 0,
        int chinese = 0,
        int math = 0,
        int english = 0,
        int science = 0
    );

    void modify(StatType type, int change);
    int get(StatType type) const;
    void show() const;

private:
    int intelligence_;
    int eq_;
    int stamina_;
    int health_;
    int stress_;
    int chinese_;
    int math_;
    int english_;
    int science_;

    static int clamp(int value);

    int& valueOf(StatType type);
    const int& valueOf(StatType type) const;
};

#endif
