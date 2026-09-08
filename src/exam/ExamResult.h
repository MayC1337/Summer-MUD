#pragma once
#include <string>

class ExamResult {
public:
    int score = 0;      // 返回的分数
    int rank = 5; // 评级：1(A) 至 5(E)
    int chineseScore = 0;
    int mathScore = 0;
    int englishScore = 0;
    int scienceScore = 0;
    std::string feedback; // 录取大学反馈

    int getscore() const {
        return score;
    }
};
