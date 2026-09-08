#include "CommandParser.h"

#include <iostream>
#include <limits>

bool CommandParser::readChoice(
    int &choice,
    int minimum,
    int maximum,
    const std::string &prompt)
{
    while (true)
    {
        std::cout << prompt;

        if (std::cin >> choice)
        {
            if (choice >= minimum && choice <= maximum)
            {
                return true;
            }

            std::cout << "请输入 " << minimum << " 到 " << maximum
                      << " 之间的数字。\n";
        }
        else
        {
            if (std::cin.eof())
            {
                return false;
            }

            std::cin.clear();
            std::cout << "输入无效，请输入数字。\n";
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}
