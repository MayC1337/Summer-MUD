#pragma once

#include "Room.h"
#include <vector>
#include <functional>

class CampusMap
{
    std::map<std::string, Room> rooms;
    std::string current = "gate";
    std::function<void()> arrivalHandler;
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
