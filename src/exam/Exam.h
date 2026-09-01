#pragma once
#include "ExamResult.h"

class Player;

class Exam {
public:
    ExamResult takeWeeklyExam(Player& player1);
    ExamResult takeFinalExam(Player& player1);
private:
    int calcRank(int score);
};
