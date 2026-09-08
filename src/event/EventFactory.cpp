#include "EventFactory.h"
#include "StoryData.h"

#include <stdexcept>
#include <vector>

Event EventFactory::createEvent(
    const std::string &id)
{
    if (const auto* node = StoryData::find(id))
    {
        std::vector<std::string> choices;
        for (const auto& choice : node->choices)
            choices.push_back(std::string(choice.text) + "（" + to_string(choice.stat) +
                (choice.delta >= 0 ? " +" : " ") + std::to_string(choice.delta) +
                "，压力 " + (choice.stress >= 0 ? "+" : "") +
                std::to_string(choice.stress) + "）");
        return Event(id, node->title, node->description, choices);
    }
    if (id == "night_study")
    {
        return Event(
            "night_study",

            "晚上十一点",

            "你看着桌上的数学试卷，"
            "又看了一眼已经十一点的时钟。",

            {"再刷一套数学卷",
             "洗澡睡觉",
             "玩一会手机"});
    }

    if (id == "classmate_help")
    {
        return Event(
            "classmate_help",

            "同桌的请求",

            "同桌拿着一道数学题问你："
            "这道题你会不会？",

            {"耐心给他讲题",
             "说自己也不会",
             "把答案直接给他"});
    }

    if (id == "teacher_talk")
    {
        return Event(
            "teacher_talk",

            "班主任谈话",

            "班主任把你叫到办公室，"
            "询问最近的学习状态。",

            {"认真听老师建议",
             "表示自己没问题",
             "沉默不说话"});
    }

    if (id == "rainy_day")
        return Event(id, "突如其来的雨", "放学时大雨倾盆，你没有带伞。",
            {"和同学共用一把伞", "冒雨跑回家", "在教室等雨停"});

    if (id == "surprise_quiz")
        return Event(id, "临时小测", "老师突然发下一张没有提前通知的试卷。",
            {"沉着独立完成", "临时翻看笔记", "安慰自己只是小测"});

    if (id == "lost_notebook")
        return Event(id, "不见的错题本", "你最重要的错题本突然找不到了。",
            {"仔细寻找", "请同学一起找", "重新整理一份"});

    if (id == "family_snack")
        return Event(id, "家人的夜宵", "晚自习结束，家人为你准备了热腾腾的夜宵。",
            {"坐下来慢慢吃", "边吃边继续学习", "留给明天"});

    if (id == "insomnia")
        return Event(id, "凌晨两点", "你躺在床上，脑子里仍然全是考试和排名。",
            {"做呼吸放松", "起来继续刷题", "拿手机转移注意"});

    if (id == "sports_injury")
        return Event(id, "体育课的小意外", "跑步时脚下一滑，脚踝传来一阵疼痛。",
            {"立刻去医务室", "休息一下继续", "假装没事"});

    if (id == "old_friend_message")
        return Event(id, "旧友的消息", "很久没联系的朋友发来一句：最近还好吗？",
            {"认真回复近况", "简单回个表情", "暂时不看消息"});

    if (id == "study_breakthrough")
        return Event(id, "突然开窍", "困扰你好几天的一类题，在这一刻忽然有了清晰思路。",
            {"趁热打铁总结方法", "马上分享给同学", "先休息庆祝一下"});

    if (id == "parent_argument")
        return Event(id, "晚饭后的争执", "家人问起周考成绩，谈话渐渐变得紧张。",
            {"耐心说明自己的计划", "忍不住顶嘴", "沉默回到房间"});

    if (id == "graduation_photo")
        return Event(id, "毕业照", "摄影师让大家站好，这可能是全班最后一张合照。",
            {"主动站到朋友身边", "安静站在最后一排", "认真记住这一刻"});

    if (id == "mock_exam_slump")
        return Event(id, "模拟考失利", "成绩单上的数字比预想低很多，你盯着它看了很久。",
            {"分析错题重新计划", "暂时放下去运动", "怀疑自己是否来得及"});

    if (id == "final_night")
        return Event(id, "高考前的信", "你翻出月初写给未来自己的信，窗外已经很安静。",
            {"写下新的回信", "和家人聊一会", "把信收好早点睡"});

    throw std::invalid_argument(
        "Unknown event id: " + id);
}
