#include <gtest/gtest.h>
#include "../event_logic.hpp"

using namespace aiworld;

TEST(EventLogic, CreateAndGetEvent) {
    EventLogic logic;
    nlohmann::json data = {{"npc", "npc_1"}, {"action", "spawn"}};
    std::string id = logic.create_event("npc_spawn", data);
    auto e = logic.get_event(id);
    ASSERT_TRUE(e.has_value());
    EXPECT_EQ(e->event_type, "npc_spawn");
    EXPECT_EQ(e->event_data["npc"], "npc_1");
}

TEST(EventLogic, DeleteEvent) {
    EventLogic logic;
    std::string id = logic.create_event("test", {{"foo", "bar"}});
    EXPECT_TRUE(logic.delete_event(id));
    EXPECT_FALSE(logic.get_event(id).has_value());
}

TEST(EventLogic, ListEvents) {
    EventLogic logic;
    logic.create_event("typeA", {{"x", 1}});
    logic.create_event("typeA", {{"x", 2}});
    logic.create_event("typeB", {{"y", 3}});
    auto listA = logic.list_events("typeA");
    EXPECT_EQ(listA.size(), 2);
    auto listB = logic.list_events("typeB");
    EXPECT_EQ(listB.size(), 1);
}