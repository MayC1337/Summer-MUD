#include "GameManager.h"
#include "TimeManager.h"
#include <iostream>

using namespace std;

class Player;

GameManager *GameManager::instance = nullptr;

GameManager::GameManager()
{
    running = false;
}

GameManager &GameManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new GameManager();
    }

    return *instance;
}

void GameManager::startGame()
{
    cout << "================" << endl;

    cout << " Summer-MUD " << endl;

    cout << " 高三人生模拟 " << endl;

    cout << "================" << endl;

    running = true;

    run();
}

void GameManager::run()
{

    while (!time.isGameEnd())
    {

        cout << "当前第";

        cout << time.getWeek();

        cout << "周" << endl;

        time.nextWeek();
    }
    endGame();
}

void GameManager::endGame()
{

    running = false;
}