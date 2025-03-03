#include "DocumentedQuery.hpp"
#include <regex>
#include <sstream>
#include <algorithm>

namespace rathena {
namespace p2p {

DocumentedQuery::DocumentedQuery(
    const std::string& name,
    const std::string& query_pattern,
    const std::vector<Parameter>& parameters,
    const Permission& permissions,
    const Impact& impact,
    const std::string& description)
    : name_(name)
    , query_pattern_(query_pattern)
    , parameters_(parameters)
    , permissions_(permissions)
    , impact_(impact)
    , description_(description) {
}

bool DocumentedQuery::validate_parameters(
    const std::vector<std::string>& params) const {
    
    // Check parameter count
    size_t required_params = std::count_if(
        parameters_.begin(),
        parameters_.end(),
        [](const Parameter& p) { return p.required; }
    );
    
    if (params.size() < required_params) {
        return false;
    }
    
    // Validate each parameter
    for (size_t i = 0; i < params.size() && i < parameters_.size(); ++i) {
        if (!validate_parameter_type(params[i], parameters_[i])) {
            return false;
        }
        
        if (!parameters_[i].validation_regex.empty() &&
            !validate_parameter_regex(params[i], parameters_[i].validation_regex)) {
            return false;
        }
    }
    
    return true;
}

bool DocumentedQuery::check_permissions(
    const std::vector<std::string>& user_roles) const {
    
    // Check if user has any of the required roles
    for (const auto& required_role : permissions_.required_roles) {
        if (std::find(user_roles.begin(), user_roles.end(),
                      required_role) == user_roles.end()) {
            return false;
        }
    }
    
    return true;
}

bool DocumentedQuery::validate_parameter_type(
    const std::string& value, const Parameter& param) const {
    
    if (param.type == "int") {
        std::regex int_regex("^-?\\d+$");
        return std::regex_match(value, int_regex);
        
    } else if (param.type == "float") {
        std::regex float_regex("^-?\\d*\\.?\\d+$");
        return std::regex_match(value, float_regex);
        
    } else if (param.type == "string") {
        return true;  // All strings are valid, length checked in regex
        
    } else if (param.type == "timestamp") {
        std::regex timestamp_regex(
            "^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}$");
        return std::regex_match(value, timestamp_regex);
    }
    
    return false;
}

bool DocumentedQuery::validate_parameter_regex(
    const std::string& value, const std::string& regex_str) const {
    
    try {
        std::regex pattern(regex_str);
        return std::regex_match(value, pattern);
    } catch (const std::regex_error& e) {
        // Log regex error
        return false;
    }
}

// Static factory methods for common queries
std::shared_ptr<DocumentedQuery> DocumentedQuery::create_character_query() {
    return std::make_shared<DocumentedQuery>(
        "get_character",
        "SELECT char_id, name, base_level, job_level, last_map "
        "FROM characters WHERE char_id = ? /* int */",
        std::vector<Parameter>{
            {"char_id", "int", "Character ID", true, "^\\d+$"}
        },
        Permission{
            true,   // read_required
            false,  // write_required
            "character_data",
            {"p2p_host"}
        },
        Impact{
            Impact::Level::LOW,
            "Read-only character data query",
            {"characters"},
            false
        },
        "Retrieve basic character information"
    );
}

std::shared_ptr<DocumentedQuery> DocumentedQuery::create_inventory_query() {
    return std::make_shared<DocumentedQuery>(
        "get_inventory",
        "SELECT nameid, amount, equip, identify "
        "FROM inventory WHERE char_id = ? /* int */",
        std::vector<Parameter>{
            {"char_id", "int", "Character ID", true, "^\\d+$"}
        },
        Permission{
            true,   // read_required
            false,  // write_required
            "inventory_data",
            {"p2p_host"}
        },
        Impact{
            Impact::Level::LOW,
            "Read-only inventory data query",
            {"inventory"},
            false
        },
        "Retrieve character inventory items"
    );
}

// QueryRegistry implementation
QueryRegistry& QueryRegistry::instance() {
    static QueryRegistry instance;
    return instance;
}

QueryRegistry::QueryRegistry() {
    register_default_queries();
}

void QueryRegistry::register_query(std::shared_ptr<DocumentedQuery> query) {
    queries_[query->get_name()] = query;
}

std::shared_ptr<DocumentedQuery> QueryRegistry::get_query(
    const std::string& name) const {
    
    auto it = queries_.find(name);
    return (it != queries_.end()) ? it->second : nullptr;
}

std::vector<std::string> QueryRegistry::list_queries() const {
    std::vector<std::string> query_names;
    query_names.reserve(queries_.size());
    
    for (const auto& pair : queries_) {
        query_names.push_back(pair.first);
    }
    
    return query_names;
}

void QueryRegistry::register_default_queries() {
    create_character_queries();
    create_inventory_queries();
    create_storage_queries();
    create_party_queries();
    create_guild_queries();
    create_mail_queries();
}

void QueryRegistry::create_character_queries() {
    register_query(DocumentedQuery::create_character_query());
}

void QueryRegistry::create_inventory_queries() {
    register_query(DocumentedQuery::create_inventory_query());
}

// Documentation generator implementation
std::string QueryDocumentationGenerator::generate_markdown(
    const DocumentedQuery& query) {
    
    std::stringstream ss;
    
    ss << "# Query: " << query.get_name() << "\n\n"
       << query.get_description() << "\n\n"
       << "## Pattern\n```sql\n" << query.get_query_pattern() << "\n```\n\n"
       << "## Parameters\n"
       << format_parameters_table(query.get_parameters()) << "\n\n"
       << "## Permissions\n"
       << format_permissions_section(query.get_permissions()) << "\n\n"
       << "## Impact\n"
       << format_impact_section(query.get_impact()) << "\n";
    
    return ss.str();
}

std::string QueryDocumentationGenerator::format_parameters_table(
    const std::vector<DocumentedQuery::Parameter>& params) {
    
    std::stringstream ss;
    
    ss << "| Name | Type | Description | Required | Validation |\n"
       << "|------|------|-------------|----------|------------|\n";
    
    for (const auto& param : params) {
        ss << "| " << param.name
           << " | " << param.type
           << " | " << param.description
           << " | " << (param.required ? "Yes" : "No")
           << " | " << (param.validation_regex.empty() ? "-" : "`" + param.validation_regex + "`")
           << " |\n";
    }
    
    return ss.str();
}

std::string QueryDocumentationGenerator::format_permissions_section(
    const DocumentedQuery::Permission& perms) {
    
    std::stringstream ss;
    
    ss << "- Scope: " << perms.scope << "\n"
       << "- Read Required: " << (perms.read_required ? "Yes" : "No") << "\n"
       << "- Write Required: " << (perms.write_required ? "Yes" : "No") << "\n"
       << "- Required Roles:\n";
    
    for (const auto& role : perms.required_roles) {
        ss << "  - " << role << "\n";
    }
    
    return ss.str();
}

std::string QueryDocumentationGenerator::format_impact_section(
    const DocumentedQuery::Impact& impact) {
    
    std::stringstream ss;
    
    ss << "- Level: ";
    switch (impact.level) {
        case DocumentedQuery::Impact::Level::LOW:
            ss << "LOW";
            break;
        case DocumentedQuery::Impact::Level::MEDIUM:
            ss << "MEDIUM";
            break;
        case DocumentedQuery::Impact::Level::HIGH:
            ss << "HIGH";
            break;
        case DocumentedQuery::Impact::Level::CRITICAL:
            ss << "CRITICAL";
            break;
    }
    
    ss << "\n- Description: " << impact.description << "\n"
       << "- Affected Tables:\n";
    
    for (const auto& table : impact.affected_tables) {
        ss << "  - " << table << "\n";
    }
    
    ss << "- Requires Sync: " << (impact.requires_sync ? "Yes" : "No") << "\n";
    
    return ss.str();
}

} // namespace p2p
} // namespace rathena