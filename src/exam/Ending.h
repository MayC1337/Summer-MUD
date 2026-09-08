#pragma once
#include "ExamResult.h"
#include <string>

class Player;
class EventManager;

class Ending
{
public:
    // 根据玩家状态 + 期末考试结果，返回结局id字符串
    std::string judgeEnding(const Player& player, const ExamResult& result);
    std::string judgeEnding(
        const Player& player,
        const ExamResult& result,
        const EventManager& eventManager);

    // 根据结局id输出结局文本到控制台
    void showEnding(const std::string& endingId);
};
