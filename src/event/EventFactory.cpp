#include "EventFactory.h"

#include <stdexcept>
#include <vector>

Event EventFactory::createEvent(
    const std::string &id)
{
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

    throw std::invalid_argument(
        "Unknown event id: " + id);
}
