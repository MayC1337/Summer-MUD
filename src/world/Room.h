#pragma once

#include <map>
#include <string>

struct Room
{
    std::string id;
    std::string name;
    std::string description;
    std::map<std::string, std::string> exits;
};
