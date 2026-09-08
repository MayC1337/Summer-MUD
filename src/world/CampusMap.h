#pragma once

#include "Room.h"
#include <vector>

class CampusMap
{
    std::map<std::string, Room> rooms;
    std::string current = "gate";
public:
    CampusMap();
    bool setLocation(const std::string& id);
    const Room& currentRoom() const;
    const std::map<std::string, Room>& getRooms() const;
    bool move(const std::string& direction);
    bool travelTo(const std::string& destination);
    std::vector<std::string> routeTo(const std::string& destination) const;
    void show() const;
    void look() const;
};
