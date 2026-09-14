#pragma once

#include <array>
#include <set>
#include <string>
#include <vector>

// 调度器的可保存进度，不包含文件操作。
struct GameProgress
{
    // Goal 仅兼容旧存档；保留枚举顺序，避免旧阶段编号错位。
    enum class Stage { DayStart, Goal, Routine, Action, Story, SpecialDay, DailyEvent, FinishDay };
    Stage stage = Stage::DayStart;
    int period = 0; // 0~3 分别代表晨间、午间、下午、晚间。
    // 以下三个目标字段仅保留 V4 存档布局，不再参与玩法。
    int dailyGoal = 1;
    int goalSubject = 6;
    int goalStart = 0;
    int lastWeeklyScore = -1;
    bool repeat = false;
    std::string location = "gate";
    std::string pendingEvent;
    std::string previousEveningLocation = "gate";
    std::array<std::vector<int>, 4> previousActions; // 最近学习日各时段的可重放菜单选择。
    std::vector<int> pendingChoices;
    std::set<std::string> npcConversations;
};
