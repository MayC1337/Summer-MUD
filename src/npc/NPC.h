#pragma once

#include <string>
#include <vector>
class EventManager;
class Player;

// 校园 NPC 的位置、出现时段和交谈入口。
class NPC
{
    std::string id;
    std::string name;
    std::string room; // 所在地点 ID，与 CampusMap::Room::id 对应。
    int period; // -1表示全天
public:
    NPC(std::string npcId, std::string npcName, std::string roomId, int availablePeriod);
    const std::string& getId() const;
    const std::string& getName() const;
    bool isPresent(const std::string& location, int currentPeriod) const;
    void talk(Player& player, const EventManager& events, bool firstConversation) const;
    static std::vector<NPC> createCampusNPCs();
};
