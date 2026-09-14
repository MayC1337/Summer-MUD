#include "core/GameManager.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
    // 程序入口仅启动总调度器，具体循环和退出处理由 GameManager 负责。
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    SetConsoleTitleW(L"铃响之前 · 高考倒计时35天");
#endif

    GameManager &game = GameManager::getInstance();
    game.startGame();

    return 0;
}
