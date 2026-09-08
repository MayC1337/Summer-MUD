#pragma once

#include <string>
#include <vector>
#include <functional>
#include <stdexcept>

struct InputInterrupted : std::runtime_error
{
    bool endOfInput;
    explicit InputInterrupted(bool eof) : std::runtime_error("Input interrupted"), endOfInput(eof) {}
};

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
