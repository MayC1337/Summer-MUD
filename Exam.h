#include <iostream>
#include <string>


using namespace std;
class Player;

class ExamResult {
private:
    int score = 0;//返回的分数
    int rank = 10e9;//排名
    string feedback;//录取大学反馈
};

class Exam {
public:
    ExamResult takeWeeklyExam(Player& player1);
    ExamResult takeFinalExam(Player& player1);
};

