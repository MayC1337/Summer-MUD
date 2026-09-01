#pragma once
#include "Player.h"
#include "ExamResult.h"
#include <string>
using namespace std;
class Player;

class Ending
{
public:
    // 根据玩家状态 + 期末考试结果，返回结局id字符串
    std::string judgeEnding(const Player& player, const ExamResult& result);

    // 根据结局id输出结局文本到控制台
    void showEnding(const std::string& endingId);
};
