#include <iostream>
#include <random>
#include "Exam.h"
#include "Player.h"

using namespace std;

int Exam::calcRank(int score) {
    if (score >= 90) return 1;    // 第1名
    else if (score >= 80) return 2; // 第2名
    else if (score >= 70) return 3; // 第3名
    else if (score >= 60) return 4; // 第4名
    else return 5; // 第5名（不及格）
}

ExamResult Exam::takeWeeklyExam(Player& player1) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(-5, 5);
    int num = dist(gen);

    Stats& stats = player1.getStats();
    int subjectability = stats.get(StatType::Chinese) + stats.get(StatType::Math) + stats.get(StatType::English) + stats.get(StatType::Science);
    int score = stats.get(StatType::intelligence) * 0.4 + subjectability * 0.4 + stats.get(StatType::Health) * 0.2 - stats.get(StatType::Stress) * 0.2 + num;

    string feedback;
    int rank = calcRank(score);
    if (score > 90) {
        feedback = "本次考试发挥十分出色！";
    } else if (score > 70) {
        feedback = "成绩中等，还有提升空间";
    } else {
        feedback = "考试成绩不理想，需要多加努力";
    }

    ExamResult result;
    result.score = score;
    result.rank = rank;
    result.feedback = feedback;
    return result;
}

ExamResult Exam::takeFinalExam(Player& player1) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(-8, 8);
    int num = dist(gen);

    Stats& stats = player1.getStats();
    int subjectability = stats.get(StatType::Chinese) + stats.get(StatType::Math) + stats.get(StatType::English) + stats.get(StatType::Science);
    int score = stats.get(StatType::intelligence) * 0.4 + subjectability * 0.4 + stats.get(StatType::Health) * 0.2 - stats.get(StatType::Stress) * 0.2 + num;

    string feedback;
    int rank = calcRank(score);
    if (score > 90) {
        feedback = "本次考试发挥十分出色！";
    } else if (score > 70) {
        feedback = "成绩中等，还有提升空间";
    } else {
        feedback = "考试成绩不理想，需要多加努力";
    }

    ExamResult result;
    result.score = score;
    result.rank = rank;
    result.feedback = feedback;
    return result;
}

