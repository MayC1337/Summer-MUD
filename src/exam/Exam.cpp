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
    int maximumScore,
    int randomBonus)
{
    // 学科基础占主要部分，身心状态只修正临场发挥，不会盖过平时积累。
    const int percentage = std::clamp(static_cast<int>(std::lround(
        stats.get(subject) * 0.77 +
        stats.get(StatType::Intelligence) * 0.12 +
        stats.get(StatType::Health) * 0.08 +
        stats.get(StatType::Stamina) * 0.03 -
        stats.get(StatType::Stress) * 0.08 + randomBonus)), 0, 100);
    return static_cast<int>(std::lround(
        percentage * maximumScore / 100.0));
}

std::string createFeedback(int score)
{
    if (score >= 690) return "顶尖发挥，你已经站在极高的分数段。";
    if (score >= 630) return "成绩非常优秀，重点高校值得冲刺。";
    if (score >= 600) return "成绩优秀，志愿选择已经相当丰富。";
    if (score >= 540) return "成绩良好，仍有继续提升的空间。";
    if (score >= 450) return "达到本科模拟线，薄弱科目仍需加强。";
    return "成绩不够理想，需要调整学习和休息安排。";
}

std::string createUniversity(int score)
{
    if (score >= 700) return "清华大学 / 北京大学";
    if (score >= 680) return "复旦大学 / 上海交通大学 / 浙江大学";
    if (score >= 650) return "南京大学 / 中国科学技术大学";
    if (score >= 630) return "武汉大学 / 华中科技大学 / 中山大学";
    if (score >= 625) return "山东大学 / 中国海洋大学（冲刺）";
    if (score >= 615) return "中国海洋大学（重点推荐）";
    if (score >= 600) return "中国石油大学（华东） / 青岛大学";
    if (score >= 570) return "省属重点大学";
    if (score >= 510) return "公办本科院校";
    if (score >= 450) return "本科院校";
    return "专科、复读或其他成长路线";
}
}

int Exam::calcRank(int score) {
    if (score >= 660) return 1;      // A
    else if (score >= 600) return 2; // B
    else if (score >= 540) return 3; // C
    else if (score >= 450) return 4; // D
    else return 5;                  // E
}

ExamResult Exam::takeWeeklyExam(Player& player1) {
    std::random_device rd;
    std::mt19937 gen(rd());
    Stats& stats = player1.getStats();
    // 周测留一点正常波动；理解力越高，成绩越稳定。
    const int fluctuation = stats.get(StatType::Intelligence) >= 80 ? 2 :
        stats.get(StatType::Intelligence) >= 60 ? 3 : 5;
    std::uniform_int_distribution<> dist(-fluctuation, fluctuation);
    ExamResult result;
    result.chineseScore = calculateSubjectScore(stats, StatType::Chinese, 150, dist(gen));
    result.mathScore = calculateSubjectScore(stats, StatType::Math, 150, dist(gen));
    result.englishScore = calculateSubjectScore(stats, StatType::English, 150, dist(gen));
    result.scienceScore = calculateSubjectScore(stats, StatType::Science, 300, dist(gen));
    const int score = (result.chineseScore + result.mathScore +
                       result.englishScore + result.scienceScore);
    int rank = calcRank(score);
    result.score = score;
    result.rank = rank;
    result.feedback = createFeedback(score);
    return result;
}

ExamResult Exam::takeFinalExam(Player& player1) {
    // 高考不再掷随机数，保证同一份最终状态得到同一个结局。
    const int num = 0;

    Stats& stats = player1.getStats();
    ExamResult result;
    result.chineseScore = calculateSubjectScore(stats, StatType::Chinese, 150, num);
    result.mathScore = calculateSubjectScore(stats, StatType::Math, 150, num);
    result.englishScore = calculateSubjectScore(stats, StatType::English, 150, num);
    result.scienceScore = calculateSubjectScore(stats, StatType::Science, 300, num);
    const int score = (result.chineseScore + result.mathScore +
                       result.englishScore + result.scienceScore);
    int rank = calcRank(score);
    result.score = score;
    result.rank = rank;
    result.feedback = createFeedback(score);
    result.university = createUniversity(score);
    return result;
}
