#pragma once
#include <iostream>
#include <string>

class ExamResult {
public:
    int score = 0;      // 返回的分数
    int rank = 10e9;    // 排名
    std::string feedback; // 录取大学反馈

    int getscore() const {
        return score;
    }
};