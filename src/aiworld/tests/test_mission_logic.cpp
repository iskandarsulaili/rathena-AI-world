#include <gtest/gtest.h>
#include "../mission_logic.hpp"

using namespace aiworld;

TEST(MissionLogic, CreateAndGetMission) {
    MissionLogic logic;
    nlohmann::json data = {{"type", "fetch"}, {"target", "item_1"}};
    std::string id = logic.create_mission("player1", data);
    auto m = logic.get_mission(id);
    ASSERT_TRUE(m.has_value());
    EXPECT_EQ(m->assignee_id, "player1");
    EXPECT_EQ(m->mission_data["type"], "fetch");
}

TEST(MissionLogic, UpdateAndDeleteMission) {
    MissionLogic logic;
    nlohmann::json data = {{"type", "kill"}, {"target", "mob_1"}};
    std::string id = logic.create_mission("player2", data);
    EXPECT_TRUE(logic.update_mission_status(id, "completed"));
    auto m = logic.get_mission(id);
    ASSERT_TRUE(m.has_value());
    EXPECT_EQ(m->status, "completed");
    EXPECT_TRUE(logic.delete_mission(id));
    EXPECT_FALSE(logic.get_mission(id).has_value());
}

TEST(MissionLogic, ListMissions) {
    MissionLogic logic;
    logic.create_mission("playerA", {{"type", "escort"}});
    logic.create_mission("playerA", {{"type", "fetch"}});
    logic.create_mission("playerB", {{"type", "kill"}});
    auto listA = logic.list_missions("playerA");
    EXPECT_EQ(listA.size(), 2);
    auto listB = logic.list_missions("playerB");
    EXPECT_EQ(listB.size(), 1);
}