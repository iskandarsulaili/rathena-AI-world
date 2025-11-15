#include <gtest/gtest.h>
#include "../entity_logic.hpp"

using namespace aiworld;

TEST(EntityLogic, UpsertAndGetEntity) {
    EntityLogic logic;
    nlohmann::json state = {{"hp", 100}, {"mp", 50}};
    logic.upsert_entity("npc1", "npc", state);
    auto e = logic.get_entity("npc1");
    ASSERT_TRUE(e.has_value());
    EXPECT_EQ(e->entity_type, "npc");
    EXPECT_EQ(e->state["hp"], 100);
}

TEST(EntityLogic, UpdateAndDeleteEntity) {
    EntityLogic logic;
    logic.upsert_entity("player1", "player", {{"level", 10}});
    logic.upsert_entity("player1", "player", {{"level", 20}});
    auto e = logic.get_entity("player1");
    ASSERT_TRUE(e.has_value());
    EXPECT_EQ(e->state["level"], 20);
    EXPECT_TRUE(logic.delete_entity("player1"));
    EXPECT_FALSE(logic.get_entity("player1").has_value());
}

TEST(EntityLogic, ListEntities) {
    EntityLogic logic;
    logic.upsert_entity("npcA", "npc", {{"x", 1}});
    logic.upsert_entity("npcB", "npc", {{"x", 2}});
    logic.upsert_entity("playerA", "player", {{"y", 3}});
    auto npcs = logic.list_entities("npc");
    EXPECT_EQ(npcs.size(), 2);
    auto players = logic.list_entities("player");
    EXPECT_EQ(players.size(), 1);
}