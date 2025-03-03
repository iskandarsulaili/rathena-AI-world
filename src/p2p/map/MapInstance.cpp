#include "MapInstance.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace rathena {
namespace p2p {

MapInstance::MapInstance(const Config& config)
    : id_(0)
    , next_entity_id_(1)
    , active_mobs_(0)
    , active_items_(0)
    , pvp_enabled_(false)
    , config_(config) {
}

MapInstance::~MapInstance() = default;

bool MapInstance::initialize(map_id_t map_id) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    try {
        id_ = map_id;
        cells_.resize(config_.height);
        for (auto& row : cells_) {
            row.resize(config_.width);
        }
        
        // Initialize spatial partitioning grid
        size_t grid_rows = (config_.height + GRID_SIZE - 1) / GRID_SIZE;
        size_t grid_cols = (config_.width + GRID_SIZE - 1) / GRID_SIZE;
        spatial_grid_.resize(grid_rows);
        for (auto& row : spatial_grid_) {
            row.resize(grid_cols);
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error initializing map " << map_id << ": " << e.what() << std::endl;
        return false;
    }
}

bool MapInstance::load_map_data(const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    try {
        size_t pos = 0;
        
        // Load cell data
        for (uint32_t y = 0; y < config_.height; y++) {
            for (uint32_t x = 0; x < config_.width; x++) {
                if (pos + 2 > data.size()) {
                    return false;
                }
                
                auto& cell = cells_[y][x];
                cell.type = data[pos++];
                cell.height = data[pos++];
                cell.flags = (data[pos] << 8) | data[pos + 1];
                pos += 2;
            }
        }
        
        // Load mob spawn points
        uint32_t spawn_count;
        std::memcpy(&spawn_count, &data[pos], sizeof(spawn_count));
        pos += sizeof(spawn_count);
        
        for (uint32_t i = 0; i < spawn_count; i++) {
            MobSpawn spawn;
            std::memcpy(&spawn.mob_id, &data[pos], sizeof(spawn.mob_id));
            pos += sizeof(spawn.mob_id);
            std::memcpy(&spawn.pos.x, &data[pos], sizeof(float));
            pos += sizeof(float);
            std::memcpy(&spawn.pos.y, &data[pos], sizeof(float));
            pos += sizeof(float);
            std::memcpy(&spawn.respawn_time, &data[pos], sizeof(spawn.respawn_time));
            pos += sizeof(spawn.respawn_time);
            std::memcpy(&spawn.level, &data[pos], sizeof(spawn.level));
            pos += sizeof(spawn.level);
            spawn.aggressive = data[pos++] != 0;
            
            if (is_mob_spawn_valid(spawn)) {
                mob_spawns_.push_back(spawn);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading map data: " << e.what() << std::endl;
        return false;
    }
}

void MapInstance::set_pvp_state(bool enabled) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    pvp_enabled_ = enabled;
}

bool MapInstance::add_entity(const Entity& entity) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    if (!validate_position(entity.pos)) {
        return false;
    }
    
    Entity new_entity = entity;
    new_entity.id = next_entity_id_++;
    
    entities_[new_entity.id] = new_entity;
    update_spatial_grid(new_entity.id, Position(), new_entity.pos);
    
    return true;
}

bool MapInstance::remove_entity(uint32_t entity_id) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    auto it = entities_.find(entity_id);
    if (it == entities_.end()) {
        return false;
    }
    
    // Remove from spatial grid
    update_spatial_grid(entity_id, it->second.pos, Position());
    
    // Remove entity
    entities_.erase(it);
    return true;
}

bool MapInstance::update_entity_position(uint32_t entity_id, const Position& pos) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    if (!validate_position(pos)) {
        return false;
    }
    
    auto it = entities_.find(entity_id);
    if (it == entities_.end()) {
        return false;
    }
    
    Position old_pos = it->second.pos;
    it->second.pos = pos;
    
    update_spatial_grid(entity_id, old_pos, pos);
    return true;
}

std::vector<Entity> MapInstance::get_entities_in_range(const Position& pos, float radius) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    std::vector<Entity> result;
    
    auto nearby_ids = get_nearby_entities(pos, radius);
    for (auto id : nearby_ids) {
        auto it = entities_.find(id);
        if (it != entities_.end() && calculate_distance(pos, it->second.pos) <= radius) {
            result.push_back(it->second);
        }
    }
    
    return result;
}

bool MapInstance::is_walkable(const Position& pos) const {
    if (!is_coordinate_in_map(pos.x, pos.y)) {
        return false;
    }
    
    uint32_t cell_x = static_cast<uint32_t>(pos.x);
    uint32_t cell_y = static_cast<uint32_t>(pos.y);
    
    return (cells_[cell_y][cell_x].type & 1) == 0;  // Bit 0: walkable
}

float MapInstance::get_terrain_height(float x, float y) const {
    if (!is_coordinate_in_map(x, y)) {
        return 0.0f;
    }
    
    uint32_t cell_x = static_cast<uint32_t>(x);
    uint32_t cell_y = static_cast<uint32_t>(y);
    
    return static_cast<float>(cells_[cell_y][cell_x].height) * 5.0f;
}

void MapInstance::update(uint32_t delta_time) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    // Update mob AI and spawns
    if (config_.mob_spawns_enabled) {
        update_mob_spawns();
        handle_mob_ai();
    }
    
    // Update item despawn timers
    update_item_despawn_timers();
    
    // Cleanup despawned entities
    cleanup_despawned_entities();
}

void MapInstance::update_spatial_grid(uint32_t entity_id, const Position& old_pos,
                                    const Position& new_pos) {
    // Remove from old grid cell
    if (is_coordinate_in_map(old_pos.x, old_pos.y)) {
        uint32_t old_grid_x = calculate_grid_cell(old_pos.x);
        uint32_t old_grid_y = calculate_grid_cell(old_pos.y);
        
        auto& old_cell = spatial_grid_[old_grid_y][old_grid_x];
        old_cell.erase(std::remove(old_cell.begin(), old_cell.end(), entity_id),
                      old_cell.end());
    }
    
    // Add to new grid cell
    if (is_coordinate_in_map(new_pos.x, new_pos.y)) {
        uint32_t new_grid_x = calculate_grid_cell(new_pos.x);
        uint32_t new_grid_y = calculate_grid_cell(new_pos.y);
        
        spatial_grid_[new_grid_y][new_grid_x].push_back(entity_id);
    }
}

std::vector<uint32_t> MapInstance::get_nearby_entities(const Position& pos, float radius) {
    std::vector<uint32_t> result;
    
    if (!is_coordinate_in_map(pos.x, pos.y)) {
        return result;
    }
    
    uint32_t grid_radius = static_cast<uint32_t>(std::ceil(radius / GRID_SIZE));
    uint32_t center_x = calculate_grid_cell(pos.x);
    uint32_t center_y = calculate_grid_cell(pos.y);
    
    for (int dy = -static_cast<int>(grid_radius); dy <= static_cast<int>(grid_radius); dy++) {
        for (int dx = -static_cast<int>(grid_radius); dx <= static_cast<int>(grid_radius); dx++) {
            int grid_x = static_cast<int>(center_x) + dx;
            int grid_y = static_cast<int>(center_y) + dy;
            
            if (grid_x >= 0 && grid_y >= 0 &&
                grid_x < static_cast<int>(spatial_grid_[0].size()) &&
                grid_y < static_cast<int>(spatial_grid_.size())) {
                
                const auto& cell = spatial_grid_[grid_y][grid_x];
                result.insert(result.end(), cell.begin(), cell.end());
            }
        }
    }
    
    return result;
}

float MapInstance::calculate_distance(const Position& a, const Position& b) const {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

uint32_t MapInstance::calculate_grid_cell(float coordinate) const {
    return static_cast<uint32_t>(coordinate) / GRID_SIZE;
}

bool MapInstance::is_coordinate_in_map(float x, float y) const {
    return x >= 0 && y >= 0 &&
           x < static_cast<float>(config_.width) &&
           y < static_cast<float>(config_.height);
}

bool MapInstance::validate_position(const Position& pos) const {
    return is_coordinate_in_map(pos.x, pos.y) && is_walkable(pos);
}

bool MapInstance::is_mob_spawn_valid(const MobSpawn& spawn) const {
    return is_coordinate_in_map(spawn.pos.x, spawn.pos.y) &&
           is_walkable(spawn.pos) &&
           spawn.mob_id != 0 &&
           spawn.respawn_time > 0;
}

} // namespace p2p
} // namespace rathena