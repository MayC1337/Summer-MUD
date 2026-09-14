#pragma once

#include <map>
#include <string>

// 地图节点；exits 将方向词映射到相邻地点 ID。
struct Room
{
    std::string id;
    std::string name;
    std::string description;
    std::map<std::string, std::string> exits;
};
