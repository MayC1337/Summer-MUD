
#include "Stats.h"
#include <iostream>
#include <stdexcept>

std::string to_string(StatType type)
{
    switch (type)
    {
    case StatType::Intelligence: return "智力";
    case StatType::EQ: return "情商";
    case StatType::Stamina: return "体力";
    case StatType::Health: return "健康";
    case StatType::Stress: return "压力";
    case StatType::Chinese: return "语文";
    case StatType::Math: return "数学";
    case StatType::English: return "英语";
    case StatType::Science: return "理综";
    }
    return "未知";
}

Stats::Stats(int intelligence, int eq, int stamina, int health, int stress,
    int chinese, int math, int english, int science)
    : intelligence_(clamp(intelligence)),
    eq_(clamp(eq)),
    stamina_(clamp(stamina)),
    health_(clamp(health)),
    stress_(clamp(stress)),
    chinese_(clamp(chinese)),
    math_(clamp(math)),
    english_(clamp(english)),
    science_(clamp(science))
{
}

int Stats::clamp(int value) const
{
    if (value < 0) return 0;
    if (value > 100) return 100;
    return value;
}

void Stats::modify(StatType type, int change)
{
    switch (type)
    {
    case StatType::Intelligence: intelligence_ = clamp(intelligence_ + change); break;
    case StatType::EQ:           eq_ = clamp(eq_ + change); break;
    case StatType::Stamina:      stamina_ = clamp(stamina_ + change); break;
    case StatType::Health:       health_ = clamp(health_ + change); break;
    case StatType::Stress:       stress_ = clamp(stress_ + change); break;
    case StatType::Chinese:      chinese_ = clamp(chinese_ + change); break;
    case StatType::Math:         math_ = clamp(math_ + change); break;
    case StatType::English:      english_ = clamp(english_ + change); break;
    case StatType::Science:      science_ = clamp(science_ + change); break;
    }
}

void Stats::set(StatType type, int value)
{
    switch (type)
    {
    case StatType::Intelligence: intelligence_ = clamp(value); break;
    case StatType::EQ:           eq_ = clamp(value); break;
    case StatType::Stamina:      stamina_ = clamp(value); break;
    case StatType::Health:       health_ = clamp(value); break;
    case StatType::Stress:       stress_ = clamp(value); break;
    case StatType::Chinese:      chinese_ = clamp(value); break;
    case StatType::Math:         math_ = clamp(value); break;
    case StatType::English:      english_ = clamp(value); break;
    case StatType::Science:      science_ = clamp(value); break;
    }
}

int Stats::get(StatType type) const
{
    switch (type)
    {
    case StatType::Intelligence: return intelligence_;
    case StatType::EQ:           return eq_;
    case StatType::Stamina:      return stamina_;
    case StatType::Health:       return health_;
    case StatType::Stress:       return stress_;
    case StatType::Chinese:      return chinese_;
    case StatType::Math:         return math_;
    case StatType::English:      return english_;
    case StatType::Science:      return science_;
    }
    throw std::invalid_argument("Unknown StatType");
}

void Stats::show() const
{
    std::cout << "----- 属性 -----\n";
    std::cout << "智力: " << intelligence_ << '\n';
    std::cout << "情商: " << eq_ << '\n';
    std::cout << "体力: " << stamina_ << '\n';
    std::cout << "健康: " << health_ << '\n';
    std::cout << "压力: " << stress_ << '\n';
    std::cout << "语文: " << chinese_ << '\n';
    std::cout << "数学: " << math_ << '\n';
    std::cout << "英语: " << english_ << '\n';
    std::cout << "理综: " << science_ << '\n';
}
