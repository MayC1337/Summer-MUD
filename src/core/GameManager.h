#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

class GameManager
{
private:
    static GameManager *instance;

    bool running;

private:
    GameManager();

public:
    static GameManager &getInstance();

    void startGame();

    void run();

    void endGame();
};

#endif