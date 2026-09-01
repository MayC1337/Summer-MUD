#include "Ending.h"
#include "../player/Player.h"
#include "ExamResult.h"
#include <iostream>
using namespace std;

std::string Ending::judgeEnding(const Player& player, const ExamResult& result)
{
    // result是期末考试ExamResult
    const int score = result.getscore();
    int health = player.getStats().get(StatType::Health); //读取玩家健康状态，可以作为结局条件
    if (health <= 0)
    {
        return "health_collapse"; //健康耗尽坏结局
    }
    else if (score >= 90)
    {
        return "ending_perfect"; //高分完美结局
    }
    else if (score >= 75)
    {
        return "ending_good"; //良好结局
    }
    else if (score >= 60)
    {
        return "ending_normal"; //及格普通结局
    }
    else
    {
        return "ending_bad"; //不及格坏结局
    }
}

void Ending::showEnding(const std::string& endingId)
{
    cout << "\n========== 游戏结局 ==========\n";
    if (endingId == "health_collapse")
    {
        cout << "你过度透支身体，健康彻底垮掉，学业被迫终止。游戏结束。\n";
    }
    else if (endingId == "ending_perfect")
    {
        cout << "你的高考成绩名列前茅！天资与努力让你收获了光明的未来。\n";
    }
    else if (endingId == "ending_good")
    {
        cout << "高考取得不错的成绩，付出有所回报，前途一片向好。\n";
    }
    else if (endingId == "ending_normal")
    {
        cout << "勉强通过高考，成绩平平，未来需要继续努力。\n";
    }
    else if (endingId == "ending_bad")
    {
        cout << "高考不及格，学业失败，这一段校园生活就此落幕。\n";
    }
    else
    {
        cout << "未知结局。\n";
    }
    cout << "==============================\n";
}
