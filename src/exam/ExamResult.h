#pragma once
#include <string>

// 考试结算值，供成绩展示、成长报告和结局判定共用。
class ExamResult {
public:
    int score = 0;      // 返回的分数
    int rank = 5; // 评级：1(A) 至 5(E)
    int chineseScore = 0;
    int mathScore = 0;
    int englishScore = 0;
    int scienceScore = 0;
    std::string feedback;
    std::string university; // 最终考试对应的模拟录取去向

    int getscore() const {
        return score;
    }
};
