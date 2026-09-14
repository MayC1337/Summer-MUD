#pragma once

#include "Room.h"
#include <vector>
#include <functional>

// 校园有向地图：管理当前位置、路径搜索和到达回调。
class CampusMap
{
    std::map<std::string, Room> rooms;
    std::string current = "gate";
    std::function<void()> arrivalHandler; // 成功到达后通知上层展示地点相关内容。
public:
    CampusMap();
    void setArrivalHandler(std::function<void()> handler);
    bool setLocation(const std::string& id);
    const Room& currentRoom() const;
    const std::map<std::string, Room>& getRooms() const;
    bool move(const std::string& direction);
    bool travelTo(const std::string& destination);
    std::vector<std::string> routeTo(const std::string& destination) const;
    void show() const;
    void look() const;
};
