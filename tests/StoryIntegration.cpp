#include "../src/action/Action.h"
#include "../src/command/CommandParser.h"
#include "../src/core/ConsoleUI.h"
#include "../src/core/TimeManager.h"
#include "../src/event/EventManager.h"
#include "../src/event/StoryData.h"
#include "../src/exam/Exam.h"
#include "../src/player/Player.h"
#include "../src/save/SaveManager.h"
#include <cassert>
#include <iostream>
#include <sstream>

int main()
{
    std::ostringstream output;
    auto* oldOutput = std::cout.rdbuf(output.rdbuf());
    std::istringstream input("1\n1\n1\n");
    auto* oldInput = std::cin.rdbuf(input.rdbuf());
    Player player("六月测试");
    EventManager events;
    events.loadEvents();
    events.triggerStory(player, 2, 1); // 同桌的邀约
    assert(events.getEventChoice("desk_1") == 1);
    events.triggerStory(player, 3, 1); // 第二天回应约定
    assert(events.getEventChoice("desk_2") == 1);
    assert(output.str().find("记得昨天的约定") != std::string::npos);

    EventManager distant;
    distant.markTriggered("desk_1");
    distant.setEventChoices({{"desk_1", 3}});
    distant.triggerStory(player, 3, 1);
    assert(output.str().find("还不太熟") != std::string::npos);

    TimeManager time;
    time.setElapsedDays(3);
    SaveManager save("story-regression-save.txt"); // 测试应在临时目录运行
    assert(save.saveGame(player, time, events));
    Player restored("empty");
    EventManager restoredEvents;
    TimeManager restoredTime;
    assert(save.loadGame(restored, restoredTime, restoredEvents));
    assert(restored.getName() == player.getName());
    assert(restoredTime.getElapsedDays() == 3);
    assert(restoredEvents.getEventChoices() == events.getEventChoices());
    assert(restoredEvents.getTriggeredEvents() == events.getTriggeredEvents());
    assert(restored.getStats().get(StatType::Math) == player.getStats().get(StatType::Math));

    // 普通行动可以重放，剧情选择不可被之前的行动输入消费。
    input.str("3\n"); input.clear(); std::cin.clear();
    CommandParser::beginActions({1, 2});
    int choice = 0;
    assert(CommandParser::readChoice(choice, 1, 3) && choice == 1);
    assert(CommandParser::readChoice(choice, 1, 3) && choice == 2);
    assert(CommandParser::endActions() == std::vector<int>({1, 2}));
    assert(CommandParser::readChoice(choice, 1, 3) && choice == 3);

    // 每个故事都有合法选项，数值保持范围内。
    assert(StoryData::nodes().size() == 16);
    for (const auto& node : StoryData::nodes())
    {
        assert(node.period >= 0 && node.period <= 3);
        assert(node.day >= 1 && node.day <= 35);
        if (node.previous[0])
        {
            const auto* previous = StoryData::find(node.previous);
            assert(previous && previous->day < node.day);
        }
    }
    Exam exam;
    for (int value : {0, 45, 75, 100})
    {
        const StatType types[] = {StatType::Intelligence, StatType::EQ, StatType::Stamina,
            StatType::Health, StatType::Chinese, StatType::Math, StatType::English, StatType::Science};
        for (StatType type : types) player.getStats().set(type, value);
        player.getStats().set(StatType::Stress, 0);
        const auto result = exam.takeFinalExam(player);
        assert(result.score == result.chineseScore + result.mathScore + result.englishScore + result.scienceScore);
        assert(result.score >= 0 && result.score <= 750);
        if (value == 100) assert(result.score == 750);
    }
    for (int period = 0; period < 4; ++period) ConsoleUI::showPeriod(period, 10, 80);
    std::istringstream rendered(output.str());
    std::string line;
    while (std::getline(rendered, line))
        if (line.rfind("║", 0) == 0 || line.rfind("╔", 0) == 0 ||
            line.rfind("╚", 0) == 0 || line.rfind("╠", 0) == 0)
            assert(ConsoleUI::displayWidth(line) == 60);
    std::cin.rdbuf(oldInput);
    std::cout.rdbuf(oldOutput);
    std::cout << "Story, save/load, replay, exam and UI checks passed.\n";
}
