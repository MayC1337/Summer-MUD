#include "Ending.h"
#include "../core/ConsoleUI.h"
#include "../event/EventManager.h"
#include "../player/Player.h"
#include "ExamResult.h"
#include <algorithm>
#include <iostream>
#include <iterator>

std::string Ending::judgeEnding(const Player& player, const ExamResult& result)
{
    const int score = result.getscore();
    const Stats& stats = player.getStats();
    const int health = stats.get(StatType::Health);
    const int stress = stats.get(StatType::Stress);
    const int eq = stats.get(StatType::EQ);
    const int intelligence = stats.get(StatType::Intelligence);
    const int subjects[] = {
        stats.get(StatType::Chinese), stats.get(StatType::Math),
        stats.get(StatType::English), stats.get(StatType::Science)};
    const auto range = std::minmax_element(std::begin(subjects), std::end(subjects));

    if (health <= 15) return "health_collapse";
    if (score >= 600 && (health <= 35 || stress >= 90)) return "ending_overdrawn";
    if (score >= 680 && health >= 60 && stress <= 70) return "ending_perfect";
    if (score >= 650 && eq < 45) return "ending_lonely_high_score";
    if (score >= 500 && *range.second >= 90 && *range.second - *range.first >= 30)
        return "ending_specialist";
    if (score >= 600 && health >= 60 && stress <= 60) return "ending_steady";
    if (score >= 570 && intelligence >= 70) return "ending_comeback";
    if (eq >= 75 && score >= 540) return "ending_youth";
    if (score < 450 && stress <= 35) return "ending_free_spirit";
    if (score >= 450) return "ending_normal";
    return "ending_bad";
}

std::string Ending::judgeEnding(
    const Player& player,
    const ExamResult& result,
    const EventManager& eventManager)
{
    const std::string baseEnding = judgeEnding(player, result);
    if (baseEnding == "health_collapse" || baseEnding == "ending_overdrawn")
    {
        return baseEnding;
    }

    if (eventManager.getEventChoice("teacher_talk") == 1 &&
        eventManager.getEventChoice("graduation_photo") == 3 &&
        eventManager.getEventChoice("final_night") == 1 &&
        result.score >= 540)
    {
        return "ending_future_self";
    }
    return baseEnding;
}

void Ending::showEnding(const std::string& endingId)
{
    std::string title;
    std::string firstLine;
    std::string secondLine;

    if (endingId == "health_collapse")
    {
        title = "病倒在考前";
        firstLine = "你把每一分钟都交给了书本，却忘了身体也需要答案。";
        secondLine = "这一次，休息比任何分数都更重要。";
    }
    else if (endingId == "ending_perfect")
    {
        title = "金榜题名";
        firstLine = "成绩、健康与心态都没有被最后一个月击垮。";
        secondLine = "走出考场时，你知道自己已经交出了最好的答卷。";
    }
    else if (endingId == "ending_overdrawn")
    {
        title = "透支的胜利";
        firstLine = "分数证明了你的坚持，疲惫也记录了它的代价。";
        secondLine = "愿下一段旅程里，你也能认真照顾自己。";
    }
    else if (endingId == "ending_lonely_high_score")
    {
        title = "孤独的高分";
        firstLine = "你赢过了许多试卷，却与身边的人渐渐疏远。";
        secondLine = "掌声响起时，你开始怀念那些没有回应的问候。";
    }
    else if (endingId == "ending_specialist")
    {
        title = "偏科天才";
        firstLine = "有一门学科成为了你最锋利的光。";
        secondLine = "未来并不只有一条标准路线，特长也能成为方向。";
    }
    else if (endingId == "ending_steady")
    {
        title = "稳稳上岸";
        firstLine = "你没有用崩溃换取进步，而是守住了自己的节奏。";
        secondLine = "稳定本身，就是最后一个月里难得的胜利。";
    }
    else if (endingId == "ending_comeback")
    {
        title = "最后的逆袭";
        firstLine = "那些一度看不懂的题，终于在最后阶段连成了线。";
        secondLine = "你用清醒和坚持，追回了曾经落下的距离。";
    }
    else if (endingId == "ending_youth")
    {
        title = "青春不留白";
        firstLine = "成绩不是故事的全部，朋友和家人也留在这个夏天。";
        secondLine = "你带走的不只是一张成绩单。";
    }
    else if (endingId == "ending_free_spirit")
    {
        title = "放飞自我";
        firstLine = "分数并不耀眼，但你没有被焦虑吞没。";
        secondLine = "方向仍需寻找，而你保留了重新出发的力气。";
    }
    else if (endingId == "ending_future_self")
    {
        title = "写给未来的自己";
        firstLine = "你重新打开那封信，发现改变的并不只是成绩。";
        secondLine = "你学会了倾听、告别，也学会了为未来作出选择。";
    }
    else if (endingId == "ending_normal")
    {
        title = "平凡也是答案";
        firstLine = "没有轰动全校的奇迹，也没有戏剧性的失败。";
        secondLine = "你平静地走出考场，未来依然有很多种写法。";
    }
    else if (endingId == "ending_bad")
    {
        title = "未完的答卷";
        firstLine = "这次成绩没有达到期待，但考试不是人生的终点。";
        secondLine = "短暂失利之后，你仍然可以选择下一条路。";
    }
    else
    {
        title = "未知结局";
        firstLine = "未来仍是一张等待书写的白纸。";
    }

    std::cout << '\n';
    ConsoleUI::boxTop("最终结局");
    ConsoleUI::boxLine("【" + title + "】");
    ConsoleUI::boxDivider();
    ConsoleUI::boxLine(firstLine);
    if (!secondLine.empty()) ConsoleUI::boxLine(secondLine);
    ConsoleUI::boxBottom();
}

void Ending::showEnding(const std::string& endingId, const ExamResult& result)
{
    showEnding(endingId);
    ConsoleUI::boxTop("模拟录取结果");
    ConsoleUI::boxLine("去向：" + result.university);
    ConsoleUI::boxDivider();
    ConsoleUI::boxLine("本结果仅用于游戏体验，不代表真实录取线。");
    ConsoleUI::boxLine("实际志愿须结合省份、年份、位次与专业。");
    ConsoleUI::boxBottom();
}
