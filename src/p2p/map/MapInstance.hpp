#ifndef MAP_INSTANCE_HPP
#define MAP_INSTANCE_HPP

#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include "../common/types.hpp"

namespace rathena {
namespace p2p {

class MapInstance {
public:
    struct Config {
        uint32_t width;
        uint32_t height;
        bool pvp_enabled;
        bool mob_spawns_enabled;
        uint32_t max_mobs;
        uint32_t max_items;
        std::string mob_db_path;
        std::string item_db_path;
    };

    struct Position {
        float x;
        float y;
        uint16_t dir;
    };

    struct Entity {
        uint32_t id;
        Position pos;
        uint8_t type;      // player, mob, item, etc.
        uint32_t status;   // alive, dead, etc.
        std::chrono::system_clock::time_point spawn_time;
    };

    struct MobSpawn {
        uint32_t mob_id;
        Position pos;
        uint32_t respawn_time;
        uint32_t level;
        bool aggressive;
    };

    struct MapCell {
        uint8_t type;      // walkable, wall, water, etc.
        uint8_t height;
        uint16_t flags;    // pvp, nowarp, noreturn, etc.
        std::vector<uint32_t> entities;
    };

    explicit MapInstance(const Config& config);
    ~MapInstance();

    // Initialization
    bool initialize(map_id_t map_id);
    bool load_map_data(const std::vector<uint8_t>& data);
    void set_pvp_state(bool enabled);

    // Entity management
    bool add_entity(const Entity& entity);
    bool remove_entity(uint32_t entity_id);
    bool update_entity_position(uint32_t entity_id, const Position& pos);
    Entity* get_entity(uint32_t entity_id);

    // Player management
    bool add_player(player_id_t player_id, const Position& pos);
    bool remove_player(player_id_t player_id);
    void update_player(player_id_t player_id);
    std::vector<player_id_t> get_nearby_players(const Position& pos, float radius);

    // Mob management
    void spawn_mobs();
    void update_mobs();
    void handle_mob_ai();
    bool damage_mob(uint32_t mob_id, uint32_t damage);

    // Item management
    uint32_t drop_item(uint32_t item_id, const Position& pos, uint32_t amount);
    bool remove_item(uint32_t item_id);
    void update_item_despawn_timers();

    // Collision and pathfinding
    bool is_walkable(const Position& pos) const;
    bool line_of_sight(const Position& from, const Position& to) const;
    std::vector<Position> find_path(const Position& from, const Position& to);
    float get_terrain_height(float x, float y) const;

    // Area queries
    std::vector<Entity> get_entities_in_range(const Position& pos, float radius);
    bool is_coordinate_in_map(float x, float y) const;
    MapCell* get_cell(uint32_t x, uint32_t y);

    // State management
    void update(uint32_t delta_time);
    std::vector<uint8_t> serialize_state() const;
    bool deserialize_state(const std::vector<uint8_t>& data);
    void clear_state();

    // Event handling
    void on_player_death(player_id_t player_id);
    void on_mob_death(uint32_t mob_id);
    void on_item_pickup(uint32_t item_id, player_id_t player_id);

private:
    // Map data
    map_id_t id_;
    std::vector<std::vector<MapCell>> cells_;
    std::unordered_map<uint32_t, Entity> entities_;
    std::vector<MobSpawn> mob_spawns_;
    
    // State tracking
    uint32_t next_entity_id_;
    uint32_t active_mobs_;
    uint32_t active_items_;
    bool pvp_enabled_;
    
    // Configuration
    Config config_;
    
    // Spatial partitioning
    static const uint32_t GRID_SIZE = 8;
    std::vector<std::vector<std::vector<uint32_t>>> spatial_grid_;
    
    void update_spatial_grid(uint32_t entity_id, const Position& old_pos, const Position& new_pos);
    std::vector<uint32_t> get_nearby_entities(const Position& pos, float radius);
    
    // Thread safety
    mutable std::mutex state_mutex_;
    
    // Helper functions
    bool validate_position(const Position& pos) const;
    void cleanup_despawned_entities();
    void update_mob_spawns();
    
    // Calculation helpers
    float calculate_distance(const Position& a, const Position& b) const;
    uint32_t calculate_grid_cell(float coordinate) const;
    bool is_mob_spawn_valid(const MobSpawn& spawn) const;
};

} // namespace p2p
} // namespace rathena

#endif // MAP_INSTANCE_HPP