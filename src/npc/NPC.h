#pragma once

#include <string>
#include <vector>
class EventManager;
class Player;

class NPC
{
    std::string id;
    std::string name;
    std::string room;
    int period; // -1表示全天
public:
    NPC(std::string npcId, std::string npcName, std::string roomId, int availablePeriod);
    const std::string& getId() const;
    const std::string& getName() const;
    bool isPresent(const std::string& location, int currentPeriod) const;
    void talk(Player& player, const EventManager& events, bool firstConversation) const;
    static std::vector<NPC> createCampusNPCs();
};
