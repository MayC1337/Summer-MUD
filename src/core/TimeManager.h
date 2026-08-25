#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

class TimeManager
{
public:
    enum class DayType
    {
        Study,
        Exam,
        Rest
    };

    static const int DAYS_PER_WEEK = 7;
    static const int DEFAULT_TOTAL_DAYS = 35;

    explicit TimeManager(int totalDays = DEFAULT_TOTAL_DAYS);

    void reset();
    bool advanceDay();

    int getTotalDays() const;
    int getElapsedDays() const;
    int getRemainingDays() const;
    int getCurrentDay() const;
    int getCurrentWeek() const;
    int getDayOfWeek() const;

    bool isFinished() const;
    bool isEndOfWeek() const;
    DayType getCurrentDayType() const;
    const char *getDayOfWeekName() const;

private:
    int totalDays;
    int elapsedDays;
};

#endif
