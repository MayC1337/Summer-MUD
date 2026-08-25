#include "GameManager.h"
#include <iostream>

using namespace std;

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

    while (running)
    {
        cout << "游戏运行中. . ." << endl;

        break;
    }
}

void GameManager::endGame()
{

    running = false;
}