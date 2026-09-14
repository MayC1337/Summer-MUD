#pragma once
#include "ExamResult.h"

class Player;

// 根据玩家当前学科能力生成周测或最终考试结果。
class Exam {
public:
    ExamResult takeWeeklyExam(Player& player1);
    ExamResult takeFinalExam(Player& player1);
private:
    int calcRank(int score);
};
