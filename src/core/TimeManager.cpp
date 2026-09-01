#include "TimeManager.h"

#include <stdexcept>

TimeManager::TimeManager(int totalDays)
    : totalDays(totalDays), elapsedDays(0)
{
    if (totalDays <= 0)
    {
        throw std::invalid_argument("totalDays must be greater than zero");
    }
}

void TimeManager::reset()
{
    elapsedDays = 0;
}

bool TimeManager::advanceDay()
{
    if (isFinished())
    {
        return false;
    }

    ++elapsedDays;
    return !isFinished();
}

int TimeManager::getTotalDays() const
{
    return totalDays;
}

int TimeManager::getElapsedDays() const
{
    return elapsedDays;
}

int TimeManager::getRemainingDays() const
{
    return totalDays - elapsedDays;
}

int TimeManager::getCurrentDay() const
{
    return isFinished() ? totalDays : elapsedDays + 1;
}

int TimeManager::getCurrentWeek() const
{
    const int dayIndex = isFinished() ? totalDays - 1 : elapsedDays;
    return dayIndex / DAYS_PER_WEEK + 1;
}

int TimeManager::getDayOfWeek() const
{
    const int dayIndex = isFinished() ? totalDays - 1 : elapsedDays;
    return dayIndex % DAYS_PER_WEEK + 1;
}

bool TimeManager::isFinished() const
{
    return elapsedDays >= totalDays;
}

bool TimeManager::isEndOfWeek() const
{
    return getDayOfWeek() == DAYS_PER_WEEK;
}

TimeManager::DayType TimeManager::getCurrentDayType() const
{
    const int dayOfWeek = getDayOfWeek();

    if (dayOfWeek == 6)
    {
        return DayType::Exam;
    }
    if (dayOfWeek == 7)
    {
        return DayType::Rest;
    }

    return DayType::Study;
}

const char *TimeManager::getDayOfWeekName() const
{
    static const char *const names[DAYS_PER_WEEK] = {
        "一", "二", "三", "四", "五", "六", "日"};

    return names[getDayOfWeek() - 1];
}
