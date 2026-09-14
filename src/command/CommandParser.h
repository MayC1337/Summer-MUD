#pragma once

#include <string>
#include <vector>
#include <functional>
#include <stdexcept>

// 以异常把 EOF/退出从任意嵌套菜单传回主游戏循环。
struct InputInterrupted : std::runtime_error
{
    bool endOfInput;
    explicit InputInterrupted(bool eof) : std::runtime_error("Input interrupted"), endOfInput(eof) {}
};

// 集中处理菜单输入，并记录可重放的行动选择以支持“沿用昨日安排”。
class CommandParser
{
public:
    static void setCommandHandler(std::function<bool(const std::string&)> handler);
    static std::vector<int> actionChoices();
    static void cancelReplay();
    static void beginActions(const std::vector<int>& replay = {});
    static std::vector<int> endActions();
    static bool readChoice(
        int &choice,
        int minimum,
        int maximum,
        const std::string &prompt = "请选择：");
};
