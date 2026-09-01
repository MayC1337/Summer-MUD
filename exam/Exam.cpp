#include <iostream>
#include "Exam.h"
#include "Player.h"
#include "Stats"
using namespace std;
class Player;
class Stats;

ExamResult Exam::takeWeeklyExam(Player& player1){
    Stats& stats = player1.getStats();
    int subjectability = stats.get(StatType::Chinese) + stats.get(StatType::Math) + stats.get(StatType::English) + stats.get(StatType::Science);
    int score = stats.get(StatType::intelligence) * 2 + subjectability + stats.get(StatType::Health) -  stats.get(StatType::Stress);
    string feedback;
    int rank;
    if(score > 600){
        rank = 1;
        feedback = "考得不错！"
    }else{
        rank = 2;
        feedback = "真棒!!!继续努力！！！😘"
    }
    ExamResult result;
    result.score = score;
    result.rank = rank;
    result.feedback = feedback;
}

ExamResult Exam::takeFinalExam(Player& player1){
    Stats& stats = player1.getStats();
    int subjectability = stats.get(StatType::Chinese) + stats.get(StatType::Math) + stats.get(StatType::English) + stats.get(StatType::Science);
    int score = stats.get(StatType::intelligence) * 2 + subjectability + stats.get(StatType::Health) -  stats.get(StatType::Stress);
    string feedback;
    int rank;
    if(score > 600){
        rank = 1;
        feedback = "恭喜你被中国海洋大学计算机类专业录取了！！！😃"
    }else{
        rank = 2;
        feedback = "恭喜你被山东大学录取了😘😘😘"
    }
    ExamResult result;
    result.score = score;
    result.rank = rank;
    result.feedback = feedback;
}

