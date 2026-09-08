#pragma once

#include <array>
#include <set>
#include <string>
#include <vector>

// 调度器的可保存进度，不包含文件操作。
struct GameProgress
{
    enum class Stage { DayStart, Goal, Routine, Action, Story, SpecialDay, DailyEvent, FinishDay };
    Stage stage = Stage::DayStart;
    int period = 0;
    int dailyGoal = 1;
    int goalSubject = 6;
    int goalStart = 0;
    int lastWeeklyScore = -1;
    bool repeat = false;
    std::string location = "gate";
    std::string pendingEvent;
    std::string previousEveningLocation = "gate";
    std::array<std::vector<int>, 4> previousActions;
    std::vector<int> pendingChoices;
    std::set<std::string> npcConversations;
};
