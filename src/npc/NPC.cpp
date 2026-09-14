#include "NPC.h"
#include "../event/EventManager.h"
#include "../player/Player.h"
#include <iostream>
#include <utility>

NPC::NPC(std::string npcId, std::string npcName, std::string roomId, int availablePeriod)
    : id(std::move(npcId)), name(std::move(npcName)), room(std::move(roomId)), period(availablePeriod) {}
const std::string& NPC::getId() const { return id; }
const std::string& NPC::getName() const { return name; }
bool NPC::isPresent(const std::string& location, int currentPeriod) const
{
    return room == location && (period == -1 || period == currentPeriod);
}

void NPC::talk(Player& player, const EventManager& events, bool firstConversation) const
{
    std::cout << name << "：";
    if (id == "linxiao")
        std::cout << (events.getEventChoice("desk_2") == 1 ?
            "还记得我们一起推导的题吗？你讲的方法我已经记住了。" : "有不会的题可以来找我，两个人想一想也许就通了。");
    else if (id == "teacher")
        std::cout << (events.hasTriggered("teacher_2") ?
            "那张便签上的方法用得怎么样？先抓住最常错的地方。" : "先把问题说具体，再一起找思路。办公室也随时欢迎你。");
    else if (id == "guard")
        std::cout << (events.hasTriggered("cat_4") ? "橘子的新窝已经收拾好了，放心吧。" : "橘子就在门边。去哪里先看看地图，晚上记得按时回家。");
    else if (id == "family")
        std::cout << (events.getEventChoice("family_2") == 1 ? "今天不问排名，只问你累不累。" : "家里给你留了饭，有什么心事也可以慢慢说。");
    else if (id == "shopkeeper") std::cout << "东西都标了价格，零食20、小说80、MP4是300、手机800。买之前看看余额。";
    else std::cout << "靠窗的位子留给认真思考的人，整理错题也别忘了休息。";
    std::cout << '\n';
    if (firstConversation)
    {
        player.modifyStat(StatType::Stress, -1);
        std::cout << "简短交流让你放松了一点：压力 -1（同一人物每天一次）。\n";
    }
    else std::cout << "今天已经聊过了，这次不再增加数值收益。\n";
}

std::vector<NPC> NPC::createCampusNPCs()
{
    return {{"linxiao", "林晓", "canteen", 1}, {"linxiao", "林晓", "classroom", 2},
        {"linxiao", "林晓", "library", 3}, {"teacher", "班主任", "classroom", 2},
        {"teacher", "班主任", "office", 3}, {"guard", "门卫", "gate", -1},
        {"family", "家人", "home", 3}, {"shopkeeper", "店主", "shop", 3},
        {"librarian", "管理员", "library", -1}};
}
