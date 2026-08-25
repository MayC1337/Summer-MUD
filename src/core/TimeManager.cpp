#include "TimeManager.h"

TimeManager::TimeManager()
{
    week = 1;
}

void TimeManager::nextWeek()
{
    week++;
}

int TimeManager::getWeek()
{
    return week;
}

bool TimeManager::isGameEnd()
{
    return week > 40;
}