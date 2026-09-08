#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include "Exam.h"
#include "../player/Player.h"

using namespace std;

namespace
{
int calculateSubjectScore(
    const Stats& stats,
    StatType subject,
    int randomBonus)
{
    int score = static_cast<int>(std::lround(
        stats.get(subject) * 0.72 +
        stats.get(StatType::Intelligence) * 0.12 +
        stats.get(StatType::Health) * 0.08 +
        stats.get(StatType::Stamina) * 0.03 -
        stats.get(StatType::Stress) * 0.08 + randomBonus));
    return std::clamp(score, 0, 100);
}

std::string createFeedback(int score)
{
    if (score >= 90) return "发挥十分出色，你已经做好了充分准备！";
    if (score >= 80) return "成绩优秀，保持现在的节奏。";
    if (score >= 70) return "成绩良好，还有继续提升的空间。";
    if (score >= 60) return "已经达到及格线，但薄弱科目仍需加强。";
    return "成绩不够理想，需要调整学习和休息安排。";
}
}

int Exam::calcRank(int score) {
    if (score >= 90) return 1;      // A
    else if (score >= 80) return 2; // B
    else if (score >= 70) return 3; // C
    else if (score >= 60) return 4; // D
    else return 5;                  // E
}

ExamResult Exam::takeWeeklyExam(Player& player1) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(-5, 5);
    Stats& stats = player1.getStats();
    ExamResult result;
    result.chineseScore = calculateSubjectScore(stats, StatType::Chinese, dist(gen));
    result.mathScore = calculateSubjectScore(stats, StatType::Math, dist(gen));
    result.englishScore = calculateSubjectScore(stats, StatType::English, dist(gen));
    result.scienceScore = calculateSubjectScore(stats, StatType::Science, dist(gen));
    const int score = (result.chineseScore + result.mathScore +
                       result.englishScore + result.scienceScore) / 4;
    int rank = calcRank(score);
    result.score = score;
    result.rank = rank;
    result.feedback = createFeedback(score);
    return result;
}

ExamResult Exam::takeFinalExam(Player& player1) {
    const int num = 0;

    Stats& stats = player1.getStats();
    ExamResult result;
    result.chineseScore = calculateSubjectScore(stats, StatType::Chinese, num);
    result.mathScore = calculateSubjectScore(stats, StatType::Math, num);
    result.englishScore = calculateSubjectScore(stats, StatType::English, num);
    result.scienceScore = calculateSubjectScore(stats, StatType::Science, num);
    const int score = (result.chineseScore + result.mathScore +
                       result.englishScore + result.scienceScore) / 4;
    int rank = calcRank(score);
    result.score = score;
    result.rank = rank;
    result.feedback = createFeedback(score);
    return result;
}
