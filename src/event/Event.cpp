#include "Event.h"
#include "StoryData.h"
#include "../player/Inventory.h"
#include "../player/Player.h"
#include "../player/Stats.h"

#include <iostream>

Event::Event(
    const std::string &eventId,
    const std::string &eventTitle,
    const std::string &eventDescription,
    const std::vector<std::string> &eventChoices)
    : id(eventId),
      title(eventTitle),
      description(eventDescription),
      choices(eventChoices)
{
}

void Event::show() const
{
    std::cout << "\n============================\n";

    std::cout << "事件：" << title << "\n";

    std::cout << description << "\n\n";

    for (std::size_t i = 0; i < choices.size(); ++i)
    {
        std::cout
            << i + 1
            << ". "
            << choices[i]
            << "\n";
    }

    std::cout << "============================\n";
}

std::string Event::getId() const
{
    return id;
}

bool Event::canTrigger(
    const Player &player) const
{
    const Stats &stats = player.getStats();
    if (id == "insomnia")
        return stats.get(StatType::Stress) >= 55;
    if (id == "sports_injury")
        return stats.get(StatType::Stamina) <= 55;
    if (id == "old_friend_message")
        return player.getInventory().hasItem("phone");
    if (id == "study_breakthrough")
        return stats.get(StatType::Intelligence) >= 45 ||
               stats.get(StatType::Math) >= 60;
    if (id == "mock_exam_slump")
        return stats.get(StatType::Stress) >= 45;
    return true;
}

void Event::applyChoice(Player &player, int choice) const
{
    // 事件选项从 1 开始编号；越界输入不产生属性变更。
    Stats &stats = player.getStats();
    if (const auto* node = StoryData::find(id))
    {
        if (choice < 1 || choice > 3) return;
        const auto& selected = node->choices[static_cast<std::size_t>(choice - 1)];
        stats.modify(selected.stat, selected.delta);
        stats.modify(StatType::Stress, selected.stress);
        std::cout << selected.reply << '\n';
        return;
    }

    if (id == "night_study")
    {
        switch (choice)
        {
        case 1:

            stats.modify(
                StatType::Math,
                5);

            stats.modify(
                StatType::Stress,
                3);

            stats.modify(
                StatType::Stamina,
                -8);

            std::cout
                << "你又刷完了一套数学卷。\n"
                << "数学 +5，压力 +3，体力 -8\n";

            break;

        case 2:

            stats.modify(
                StatType::Health,
                3);

            stats.modify(
                StatType::Stress,
                -3);

            std::cout
                << "你决定早点睡觉。\n"
                << "健康 +3，压力 -3\n";

            break;

        case 3:

            stats.modify(
                StatType::Stress,
                -8);

            stats.modify(
                StatType::Stamina,
                -2);

            std::cout
                << "你玩了一会手机。\n"
                << "压力 -8，体力 -2\n";

            break;
        }
        return;
    }

    if (id == "classmate_help")
    {
        switch (choice)
        {
        case 1:
            stats.modify(StatType::EQ, 5);
            stats.modify(StatType::Math, 3);
            stats.modify(StatType::Stress, -2);
            std::cout << "你耐心讲清了题目。情商 +5，数学 +3，压力 -2\n";
            break;
        case 2:
            stats.modify(StatType::EQ, -2);
            stats.modify(StatType::Stress, -3);
            std::cout << "你婉拒了同桌。情商 -2，压力 -3\n";
            break;
        case 3:
            stats.modify(StatType::EQ, 1);
            stats.modify(StatType::Stress, 2);
            std::cout << "你直接给出了答案。情商 +1，压力 +2\n";
            break;
        }
        return;
    }

    if (id == "teacher_talk")
    {
        switch (choice)
        {
        case 1:
            stats.modify(StatType::Intelligence, 3);
            stats.modify(StatType::Stress, -5);
            std::cout << "老师的建议让你理清了计划。智力 +3，压力 -5\n";
            break;
        case 2:
            stats.modify(StatType::Stress, 5);
            std::cout << "你嘴上说没问题，心里却更紧张了。压力 +5\n";
            break;
        case 3:
            stats.modify(StatType::EQ, -3);
            stats.modify(StatType::Stress, 3);
            std::cout << "沉默让谈话有些尴尬。情商 -3，压力 +3\n";
            break;
        }
        return;
    }

    if (id == "rainy_day")
    {
        if (choice == 1) { stats.modify(StatType::EQ, 4); stats.modify(StatType::Stress, -3); std::cout << "一路的闲聊让雨天也变得轻松。情商 +4，压力 -3\n"; }
        if (choice == 2) { stats.modify(StatType::Health, -5); stats.modify(StatType::Stamina, -8); std::cout << "你浑身湿透地跑回家。健康 -5，体力 -8\n"; }
        if (choice == 3) { stats.modify(StatType::Intelligence, 1); stats.modify(StatType::Stress, -2); std::cout << "你在空教室整理了思绪。智力 +1，压力 -2\n"; }
        return;
    }

    if (id == "surprise_quiz")
    {
        if (choice == 1) { stats.modify(StatType::Intelligence, 2); stats.modify(StatType::Chinese, 2); stats.modify(StatType::Stress, 5); std::cout << "你独立完成了小测。智力 +2，语文 +2，压力 +5\n"; }
        if (choice == 2) { stats.modify(StatType::Stress, 3); stats.modify(StatType::EQ, -2); std::cout << "翻笔记让你更加心虚。压力 +3，情商 -2\n"; }
        if (choice == 3) { stats.modify(StatType::Stress, -5); std::cout << "你很快调整好了心态。压力 -5\n"; }
        return;
    }

    if (id == "lost_notebook")
    {
        if (choice == 1) { stats.modify(StatType::Intelligence, 2); stats.modify(StatType::Stress, 3); std::cout << "你最终在书堆里找到了它。智力 +2，压力 +3\n"; }
        if (choice == 2) { stats.modify(StatType::EQ, 4); stats.modify(StatType::Stress, -2); std::cout << "同学很快帮你找回了本子。情商 +4，压力 -2\n"; }
        if (choice == 3) { stats.modify(StatType::Intelligence, 3); stats.modify(StatType::Stress, 6); std::cout << "重新整理让知识更加清晰。智力 +3，压力 +6\n"; }
        return;
    }

    if (id == "family_snack")
    {
        if (choice == 1) { stats.modify(StatType::Health, 5); stats.modify(StatType::Stress, -8); std::cout << "一顿热饭让你重新安定下来。健康 +5，压力 -8\n"; }
        if (choice == 2) { stats.modify(StatType::Math, 3); stats.modify(StatType::Stamina, 5); stats.modify(StatType::Stress, 3); std::cout << "你边吃边完成了练习。数学 +3，体力 +5，压力 +3\n"; }
        if (choice == 3) { stats.modify(StatType::Stamina, 4); std::cout << "你把夜宵留作明早的能量。体力 +4\n"; }
        return;
    }

    if (id == "insomnia")
    {
        if (choice == 1) { stats.modify(StatType::Stress, -12); stats.modify(StatType::Health, 2); std::cout << "呼吸渐渐平稳，你终于睡着。压力 -12，健康 +2\n"; }
        if (choice == 2) { stats.modify(StatType::Math, 4); stats.modify(StatType::Stress, 12); stats.modify(StatType::Stamina, -12); std::cout << "深夜刷题换来了进度，也透支了身体。数学 +4，压力 +12，体力 -12\n"; }
        if (choice == 3) { stats.modify(StatType::Stress, -5); stats.modify(StatType::Stamina, -5); std::cout << "短暂分心后困意终于到来。压力 -5，体力 -5\n"; }
        return;
    }

    if (id == "sports_injury")
    {
        if (choice == 1) { player.changeMoney(-50); stats.modify(StatType::Health, 4); stats.modify(StatType::Stress, -2); std::cout << "校医及时处理了伤处。金钱 -50，健康 +4，压力 -2\n"; }
        if (choice == 2) { stats.modify(StatType::Health, -8); stats.modify(StatType::Stress, 3); std::cout << "勉强继续让伤处更疼了。健康 -8，压力 +3\n"; }
        if (choice == 3) { stats.modify(StatType::Health, -12); stats.modify(StatType::Stress, 5); std::cout << "逞强付出了代价。健康 -12，压力 +5\n"; }
        return;
    }

    if (id == "old_friend_message")
    {
        if (choice == 1) { stats.modify(StatType::EQ, 6); stats.modify(StatType::Stress, -8); std::cout << "久违的交流让你感到温暖。情商 +6，压力 -8\n"; }
        if (choice == 2) { stats.modify(StatType::EQ, 1); stats.modify(StatType::Stress, -2); std::cout << "简单的回应也维系了联系。情商 +1，压力 -2\n"; }
        if (choice == 3) { stats.modify(StatType::Stress, 2); std::cout << "未读消息一直留在你的心里。压力 +2\n"; }
        return;
    }

    if (id == "study_breakthrough")
    {
        if (choice == 1) { stats.modify(StatType::Intelligence, 4); stats.modify(StatType::Math, 5); stats.modify(StatType::Stress, 5); std::cout << "你把方法完整写进了错题本。智力 +4，数学 +5，压力 +5\n"; }
        if (choice == 2) { stats.modify(StatType::EQ, 5); stats.modify(StatType::Math, 3); std::cout << "讲给同学后，你理解得更牢了。情商 +5，数学 +3\n"; }
        if (choice == 3) { stats.modify(StatType::Stress, -10); stats.modify(StatType::Health, 2); std::cout << "你允许自己享受这一刻。压力 -10，健康 +2\n"; }
        return;
    }

    if (id == "parent_argument")
    {
        if (choice == 1) { stats.modify(StatType::EQ, 5); stats.modify(StatType::Stress, -5); std::cout << "坦诚沟通换来了理解。情商 +5，压力 -5\n"; }
        if (choice == 2) { stats.modify(StatType::EQ, -5); stats.modify(StatType::Stress, 12); std::cout << "争吵让所有人都沉默下来。情商 -5，压力 +12\n"; }
        if (choice == 3) { stats.modify(StatType::Stress, 5); std::cout << "没有说出口的话压在心里。压力 +5\n"; }
        return;
    }

    if (id == "graduation_photo")
    {
        if (choice == 1) { stats.modify(StatType::EQ, 6); stats.modify(StatType::Stress, -6); std::cout << "快门按下时，大家都笑得很真诚。情商 +6，压力 -6\n"; }
        if (choice == 2) { stats.modify(StatType::Stress, -2); std::cout << "你安静地留在了这段回忆里。压力 -2\n"; }
        if (choice == 3) { stats.modify(StatType::Intelligence, 2); stats.modify(StatType::EQ, 3); std::cout << "你认真记住了身边每张脸。智力 +2，情商 +3\n"; }
        return;
    }

    if (id == "mock_exam_slump")
    {
        if (choice == 1) { stats.modify(StatType::Intelligence, 4); stats.modify(StatType::Stress, 5); std::cout << "你把失利变成了新的计划。智力 +4，压力 +5\n"; }
        if (choice == 2) { stats.modify(StatType::Health, 4); stats.modify(StatType::Stress, -10); std::cout << "运动让你重新找回节奏。健康 +4，压力 -10\n"; }
        if (choice == 3) { stats.modify(StatType::Stress, 15); stats.modify(StatType::Health, -3); std::cout << "怀疑不断放大。压力 +15，健康 -3\n"; }
        return;
    }

    if (id == "final_night")
    {
        if (choice == 1) { stats.modify(StatType::Intelligence, 3); stats.modify(StatType::Stress, -5); std::cout << "你写下了比成绩更重要的答案。智力 +3，压力 -5\n"; }
        if (choice == 2) { stats.modify(StatType::EQ, 5); stats.modify(StatType::Stress, -8); std::cout << "家人的声音让你平静下来。情商 +5，压力 -8\n"; }
        if (choice == 3) { stats.modify(StatType::Health, 4); stats.modify(StatType::Stress, -12); std::cout << "你关掉台灯，为明天保存体力。健康 +4，压力 -12\n"; }
    }
}
