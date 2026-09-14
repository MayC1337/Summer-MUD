#include "CampusMap.h"
#include "../core/ConsoleUI.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
#include <utility>

CampusMap::CampusMap()
{
    const Room definitions[] = {
        {"office", "办公室", "粉笔灰落在教案旁，门口贴着答疑时间。", {{"east", "classroom"}, {"south", "gym"}}},
        {"classroom", "教室", "黑板上的倒计时又少了一天，同桌的笔记摊在桌上。", {{"west", "office"}, {"east", "library"}, {"south", "gate"}}},
        {"library", "图书馆", "靠窗座位很安静，错题本和参考书整齐摆放。", {{"west", "classroom"}, {"south", "canteen"}}},
        {"gym", "体育馆", "球落地的声音从门内传来，操场上的风很轻。", {{"north", "office"}, {"east", "gate"}, {"south", "arcade"}}},
        {"gate", "校门", "门卫室旁有一只叫橘子的猫，路口通向校园各处。", {{"north", "classroom"}, {"west", "gym"}, {"east", "canteen"}, {"south", "home"}}},
        {"canteen", "食堂", "热饭的香味飘出窗口，午间这里总是很热闹。", {{"north", "library"}, {"west", "gate"}, {"south", "shop"}}},
        {"arcade", "游戏厅", "街机屏幕闪着光，墙上的钟提醒你留意时间。", {{"north", "gym"}, {"east", "home"}}},
        {"home", "家", "书桌旁留着一盏灯，家人准备好了热水。", {{"north", "gate"}, {"west", "arcade"}, {"east", "shop"}}},
        {"shop", "商店", "柜台上摆着零食、小说和学习之余的娱乐用品。", {{"north", "canteen"}, {"west", "home"}}}
    };
    for (const auto& room : definitions) rooms.emplace(room.id, room);
}

void CampusMap::setArrivalHandler(std::function<void()> handler)
{
    arrivalHandler = std::move(handler);
}

bool CampusMap::setLocation(const std::string& id)
{
    if (!rooms.count(id)) return false;
    current = id;
    return true;
}
const Room& CampusMap::currentRoom() const { return rooms.at(current); }
const std::map<std::string, Room>& CampusMap::getRooms() const { return rooms; }

bool CampusMap::move(const std::string& direction)
{
    const auto exit = currentRoom().exits.find(direction);
    if (exit == currentRoom().exits.end())
    {
        std::cout << "这个方向没有出口。\n";
        return false;
    }
    current = exit->second;
    look();
    if (arrivalHandler) arrivalHandler();
    return true;
}

std::vector<std::string> CampusMap::routeTo(const std::string& destination) const
{
    if (!rooms.count(destination)) return {};
    std::queue<std::string> pending;
    std::map<std::string, std::string> previous;
    pending.push(current);
    previous[current] = "";
    while (!pending.empty())
    {
        const auto id = pending.front();
        pending.pop();
        if (id == destination) break;
        for (const auto& exit : rooms.at(id).exits)
            if (!previous.count(exit.second))
            {
                previous[exit.second] = id;
                pending.push(exit.second);
            }
    }
    if (!previous.count(destination)) return {};
    std::vector<std::string> route;
    for (std::string id = destination; !id.empty(); id = previous.at(id)) route.push_back(id);
    std::reverse(route.begin(), route.end());
    return route;
}

bool CampusMap::travelTo(const std::string& destination)
{
    const auto route = routeTo(destination);
    if (route.empty()) return false;
    std::cout << "路线：";
    for (std::size_t i = 0; i < route.size(); ++i)
        std::cout << (i ? " → " : "") << rooms.at(route[i]).name;
    std::cout << '\n';
    current = destination;
    if (arrivalHandler) arrivalHandler();
    return true;
}

void CampusMap::look() const
{
    ConsoleUI::boxTop(currentRoom().name);
    ConsoleUI::boxLine(currentRoom().description);
    std::string exits = "出口：";
    for (const auto& exit : currentRoom().exits)
        exits += exit.first + " " + rooms.at(exit.second).name + "  ";
    ConsoleUI::boxBottom();
    std::cout << exits << '\n';
}

void CampusMap::show() const
{
    ConsoleUI::boxTop("校园地图 · 上北下南，左西右东");
    ConsoleUI::boxLine("办公室 -------- 教室 -------- 图书馆");
    ConsoleUI::boxLine("  |              |              |");
    ConsoleUI::boxLine("体育馆 -------- 校门 ---------- 食堂");
    ConsoleUI::boxLine("  |              |              |");
    ConsoleUI::boxLine("游戏厅 --------  家 ----------- 商店");
    ConsoleUI::boxDivider();
    ConsoleUI::boxLine("你在：" + currentRoom().name);
    ConsoleUI::boxLine("晚间可输入 north/south/east/west 或 北/南/东/西");
    ConsoleUI::boxLine("移动不耗行动；到达后选6进行当地活动。");
    ConsoleUI::boxBottom();
}
