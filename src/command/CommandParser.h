#pragma once

#include <string>

class CommandParser
{
public:
    static bool readChoice(
        int &choice,
        int minimum,
        int maximum,
        const std::string &prompt = "请选择：");
};
