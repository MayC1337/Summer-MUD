#include "GameManager.h"

int main()
{
    GameManager &game = GameManager::getInstance();

    game.startGame();

    return 0;
}