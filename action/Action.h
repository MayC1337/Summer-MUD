#pragma once

#include "Player.h"
#include <string>

enum class ActionType {
    Study,
    Rest,
    Entertainment,
    Socialize,
    Exercise,
    Shopping
};

enum class StudySubject {
    Chinese,
    Math,
    English,
    Physics,
    Chemistry,
    Biology
};

class Action {
public:
    virtual ~Action() = default;

    // 主程序统一调用的入口。
    virtual bool executeDailyAction(Player& player) = 0;
    virtual ActionType getType() const = 0;
    virtual std::string getName() const = 0;

    // 兼容类图中的基础行动接口。
    static bool study(Player& player, StudySubject subject);
    static bool rest(Player& player);
    static bool entertain(Player& player);
    static bool socialize(Player& player);

protected:
    static void applyStress(Player& player, int delta);
    static void applyHealth(Player& player, int delta);
    static int scaledLearningGain(const Player& player, int baseGain);
};

// 第一阶段建议主程序直接实例化这些具体 Action。
class StudyAction : public Action {
private:
    StudySubject subject_;
    bool selfStudy_;
public:
    explicit StudyAction(StudySubject subject, bool selfStudy = false)
        : subject_(subject), selfStudy_(selfStudy) {}

    bool executeDailyAction(Player& player) override;
    ActionType getType() const override { return ActionType::Study; }
    std::string getName() const override { return selfStudy_ ? "自习" : "认真听课"; }
};

class RestAction : public Action {
public:
    bool executeDailyAction(Player& player) override;
    ActionType getType() const override { return ActionType::Rest; }
    std::string getName() const override { return "休息"; }
};

class EntertainmentAction : public Action {
public:
    enum class Mode { Phone, MP4, Novel, Snack, GameHall };
private:
    Mode mode_;
public:
    explicit EntertainmentAction(Mode mode) : mode_(mode) {}
    bool executeDailyAction(Player& player) override;
    ActionType getType() const override { return ActionType::Entertainment; }
    std::string getName() const override;
};

class SocializeAction : public Action {
public:
    enum class Mode { Chat, DiscussStudy, HelpFriend };
private:
    Mode mode_;
    StudySubject subject_;
public:
    explicit SocializeAction(Mode mode, StudySubject subject = StudySubject::Math)
        : mode_(mode), subject_(subject) {}
    bool executeDailyAction(Player& player) override;
    ActionType getType() const override { return ActionType::Socialize; }
    std::string getName() const override;
};

class ExerciseAction : public Action {
public:
    bool executeDailyAction(Player& player) override;
    ActionType getType() const override { return ActionType::Exercise; }
    std::string getName() const override { return "体育锻炼"; }
};
