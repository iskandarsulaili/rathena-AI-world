#include "DocumentedQuery.hpp"
#include <memory>
#include <vector>

namespace rathena {
namespace p2p {

// Character Queries
void QueryRegistry::create_character_queries() {
    // Basic character data
    std::vector<DocumentedQuery::Parameter> char_params = {
        {"char_id", "int", "Character ID", true, "^\\d+$"}
    };

    DocumentedQuery::Permission char_perm{
        true,   // read_required
        false,  // write_required
        "character_data",
        {"p2p_host"}
    };

    DocumentedQuery::Impact char_impact{
        DocumentedQuery::Impact::Level::LOW,
        "Read basic character data",
        {"characters"},
        false
    };

    register_query(std::make_shared<DocumentedQuery>(
        "get_character_data",
        "SELECT char_id, account_id, char_num, name, class, base_level, job_level, "
        "base_exp, job_exp, zeny, str, agi, vit, int, dex, luk, max_hp, hp, "
        "max_sp, sp, status_point, skill_point, party_id, guild_id, pet_id, "
        "homun_id, faction_id, last_map, last_x, last_y, save_map, save_x, save_y "
        "FROM characters WHERE char_id = ? /* int */",
        char_params,
        char_perm,
        char_impact,
        "Retrieve comprehensive character information"
    ));

    // Character status
    std::vector<DocumentedQuery::Parameter> status_params = {
        {"hp", "int", "Current HP", true, "^\\d+$"},
        {"sp", "int", "Current SP", true, "^\\d+$"},
        {"last_map", "string", "Current map name", true, "^[a-zA-Z0-9_-]+$"},
        {"last_x", "int", "X coordinate", true, "^\\d+$"},
        {"last_y", "int", "Y coordinate", true, "^\\d+$"},
        {"char_id", "int", "Character ID", true, "^\\d+$"}
    };

    DocumentedQuery::Permission status_perm{
        false,  // read_required
        true,   // write_required
        "character_status",
        {"p2p_host"}
    };

    DocumentedQuery::Impact status_impact{
        DocumentedQuery::Impact::Level::MEDIUM,
        "Update character status and position",
        {"characters"},
        true
    };

    register_query(std::make_shared<DocumentedQuery>(
        "update_character_status",
        "UPDATE characters SET hp = ?, sp = ?, last_map = ?, last_x = ?, last_y = ? "
        "WHERE char_id = ? /* int */ /* int */ /* string */ /* int */ /* int */ /* int */",
        status_params,
        status_perm,
        status_impact,
        "Update character's current status and position"
    ));
}

// Inventory Queries
void QueryRegistry::create_inventory_queries() {
    // Get inventory items
    std::vector<DocumentedQuery::Parameter> inv_params = {
        {"char_id", "int", "Character ID", true, "^\\d+$"}
    };

    DocumentedQuery::Permission inv_perm{
        true,   // read_required
        false,  // write_required
        "inventory_data",
        {"p2p_host"}
    };

    DocumentedQuery::Impact inv_impact{
        DocumentedQuery::Impact::Level::LOW,
        "Read inventory items",
        {"inventory"},
        false
    };

    register_query(std::make_shared<DocumentedQuery>(
        "get_inventory_items",
        "SELECT id, nameid, amount, equip, identify, refine, attribute, card0, "
        "card1, card2, card3, expire_time, bound, unique_id "
        "FROM inventory WHERE char_id = ? /* int */",
        inv_params,
        inv_perm,
        inv_impact,
        "Retrieve character's inventory items"
    ));

    // Update item amount
    std::vector<DocumentedQuery::Parameter> update_params = {
        {"amount", "int", "New amount", true, "^\\d+$"},
        {"item_id", "int", "Item ID", true, "^\\d+$"},
        {"char_id", "int", "Character ID", true, "^\\d+$"}
    };

    DocumentedQuery::Permission update_perm{
        false,  // read_required
        true,   // write_required
        "inventory_modify",
        {"p2p_host"}
    };

    DocumentedQuery::Impact update_impact{
        DocumentedQuery::Impact::Level::MEDIUM,
        "Update item quantity",
        {"inventory"},
        true
    };

    register_query(std::make_shared<DocumentedQuery>(
        "update_item_amount",
        "UPDATE inventory SET amount = ? WHERE id = ? AND char_id = ? /* int */ /* int */ /* int */",
        update_params,
        update_perm,
        update_impact,
        "Update inventory item amount"
    ));
}

// Storage Queries
void QueryRegistry::create_storage_queries() {
    std::vector<DocumentedQuery::Parameter> storage_params = {
        {"account_id", "int", "Account ID", true, "^\\d+$"}
    };

    DocumentedQuery::Permission storage_perm{
        true,   // read_required
        false,  // write_required
        "storage_data",
        {"p2p_host"}
    };

    DocumentedQuery::Impact storage_impact{
        DocumentedQuery::Impact::Level::LOW,
        "Read storage contents",
        {"storage"},
        false
    };

    register_query(std::make_shared<DocumentedQuery>(
        "get_storage_items",
        "SELECT id, nameid, amount, identify, refine, attribute, card0, card1, "
        "card2, card3, expire_time, bound, unique_id "
        "FROM storage WHERE account_id = ? /* int */",
        storage_params,
        storage_perm,
        storage_impact,
        "Retrieve account storage items"
    ));
}

// Party Queries
void QueryRegistry::create_party_queries() {
    std::vector<DocumentedQuery::Parameter> party_params = {
        {"party_id", "int", "Party ID", true, "^\\d+$"}
    };

    DocumentedQuery::Permission party_perm{
        true,   // read_required
        false,  // write_required
        "party_data",
        {"p2p_host"}
    };

    DocumentedQuery::Impact party_impact{
        DocumentedQuery::Impact::Level::LOW,
        "Read party member information",
        {"party_member", "characters"},
        false
    };

    register_query(std::make_shared<DocumentedQuery>(
        "get_party_members",
        "SELECT c.char_id, c.name, c.base_level, c.job_level, c.last_map, "
        "c.online, p.leader, p.party_id "
        "FROM party_member p "
        "JOIN characters c ON c.char_id = p.char_id "
        "WHERE p.party_id = ? /* int */",
        party_params,
        party_perm,
        party_impact,
        "Retrieve party member information"
    ));
}

} // namespace p2p
} // namespace rathena