#ifndef DOCUMENTED_QUERY_HPP
#define DOCUMENTED_QUERY_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace rathena {
namespace p2p {

class DocumentedQuery {
public:
    struct Parameter {
        std::string name;
        std::string type;
        std::string description;
        bool required;
        std::string validation_regex;
    };

    struct Permission {
        bool read_required;
        bool write_required;
        std::string scope;
        std::vector<std::string> required_roles;
    };

    struct Impact {
        enum class Level {
            LOW,
            MEDIUM,
            HIGH,
            CRITICAL
        };

        Level level;
        std::string description;
        std::vector<std::string> affected_tables;
        bool requires_sync;
    };

    // Constructor
    DocumentedQuery(
        const std::string& name,
        const std::string& query_pattern,
        const std::vector<Parameter>& parameters,
        const Permission& permissions,
        const Impact& impact,
        const std::string& description
    );

    // Getters
    const std::string& get_name() const { return name_; }
    const std::string& get_query_pattern() const { return query_pattern_; }
    const std::vector<Parameter>& get_parameters() const { return parameters_; }
    const Permission& get_permissions() const { return permissions_; }
    const Impact& get_impact() const { return impact_; }
    const std::string& get_description() const { return description_; }

    // Query validation
    bool validate_parameters(const std::vector<std::string>& params) const;
    bool check_permissions(const std::vector<std::string>& user_roles) const;

    // Static factory methods
    static std::shared_ptr<DocumentedQuery> create_character_query();
    static std::shared_ptr<DocumentedQuery> create_inventory_query();
    static std::shared_ptr<DocumentedQuery> create_storage_query();
    static std::shared_ptr<DocumentedQuery> create_party_query();
    static std::shared_ptr<DocumentedQuery> create_guild_query();
    static std::shared_ptr<DocumentedQuery> create_mail_query();

private:
    std::string name_;
    std::string query_pattern_;
    std::vector<Parameter> parameters_;
    Permission permissions_;
    Impact impact_;
    std::string description_;

    // Validation helpers
    bool validate_parameter_type(const std::string& value, const Parameter& param) const;
    bool validate_parameter_regex(const std::string& value, const std::string& regex) const;
};

// Query Registry
class QueryRegistry {
public:
    static QueryRegistry& instance();

    void register_query(std::shared_ptr<DocumentedQuery> query);
    std::shared_ptr<DocumentedQuery> get_query(const std::string& name) const;
    std::vector<std::string> list_queries() const;
    
    // Template queries for common operations
    void register_default_queries();

private:
    QueryRegistry();
    ~QueryRegistry() = default;
    QueryRegistry(const QueryRegistry&) = delete;
    QueryRegistry& operator=(const QueryRegistry&) = delete;

    std::unordered_map<std::string, std::shared_ptr<DocumentedQuery>> queries_;

    // Default query creators
    void create_character_queries();
    void create_inventory_queries();
    void create_storage_queries();
    void create_party_queries();
    void create_guild_queries();
    void create_mail_queries();
};

// Documentation generator
class QueryDocumentationGenerator {
public:
    static std::string generate_markdown(const DocumentedQuery& query);
    static std::string generate_html(const DocumentedQuery& query);
    static std::string generate_sql_comment(const DocumentedQuery& query);

private:
    static std::string format_parameters_table(const std::vector<DocumentedQuery::Parameter>& params);
    static std::string format_permissions_section(const DocumentedQuery::Permission& perms);
    static std::string format_impact_section(const DocumentedQuery::Impact& impact);
};

} // namespace p2p
} // namespace rathena

#endif // DOCUMENTED_QUERY_HPP