#include "CommandParser.h"

#include <iostream>
#include <sstream>
#include <utility>

namespace
{
bool recording = false;
std::vector<int> recorded;
std::vector<int> replayed;
std::size_t replayIndex = 0;
std::function<bool(const std::string&)> commandHandler;
}

void CommandParser::setCommandHandler(std::function<bool(const std::string&)> handler)
{
    commandHandler = std::move(handler);
}

std::vector<int> CommandParser::actionChoices()
{
    return recording ? recorded : std::vector<int>{};
}

void CommandParser::cancelReplay()
{
    replayed.clear();
}

void CommandParser::beginActions(const std::vector<int>& replay)
{
    recording = true;
    recorded.clear();
    replayed = replay;
    replayIndex = 0;
}

std::vector<int> CommandParser::endActions()
{
    recording = false;
    replayed.clear();
    return recorded;
}

bool CommandParser::readChoice(
    int &choice,
    int minimum,
    int maximum,
    const std::string &prompt)
{
    while (true)
    {
        if (recording && replayIndex < replayed.size())
        {
            choice = replayed[replayIndex++];
            if (choice >= minimum && choice <= maximum)
            {
                std::cout << prompt << choice << "（沿用安排）\n";
                recorded.push_back(choice);
                return true;
            }
            replayed.clear();
        }
        std::cout << prompt;

        std::string line;
        if (!std::getline(std::cin, line))
        {
            if (commandHandler) throw InputInterrupted(true);
            return false;
        }
        const auto first = line.find_first_not_of(" \t\r");
        line = first == std::string::npos ? "" : line.substr(first, line.find_last_not_of(" \t\r") - first + 1);
        if (commandHandler && commandHandler(line)) continue;
        if (!commandHandler && (line == "help" || line == "帮助"))
        {
            std::cout << "请输入菜单中一个完整的整数选项。\n";
            continue;
        }
        std::istringstream parser(line);
        std::string extra;
        if ((parser >> choice) && !(parser >> extra) && choice >= minimum && choice <= maximum)
        {
            if (recording) recorded.push_back(choice);
            return true;
        }
        std::cout << "输入无效，请输入 " << minimum << " 到 " << maximum
                  << " 之间的完整整数，或输入 help 查看命令。\n";
    }
}
