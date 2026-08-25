#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

class TimeManager
{

private:
    int week;

public:
    TimeManager();

    void nextWeek();

    int getWeek();

    bool isGameEnd();
};

#endif