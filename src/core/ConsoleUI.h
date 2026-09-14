#pragma once

#include <string>

namespace ConsoleUI
{
constexpr int BOX_WIDTH = 58;

int displayWidth(const std::string &text);
bool isInteractive();
void message(const std::string& text, bool error = false);

void clearScreen();
void typeLine(const std::string &text, int delayMilliseconds = 12);
void showSplash();
void showPeriod(int period, int stamina, int stress);
void waitForExit(const std::string &message = "输入 0 退出：");

void boxTop(const std::string &title = std::string(), int width = BOX_WIDTH);
void boxDivider(const std::string &title = std::string(), int width = BOX_WIDTH);
void boxLine(const std::string &text, int width = BOX_WIDTH);
void boxBottom(int width = BOX_WIDTH);

std::string makeBar(int value, int maximum = 100, int cells = 10);
}
