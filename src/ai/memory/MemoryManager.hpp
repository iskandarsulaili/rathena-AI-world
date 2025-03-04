#ifndef MEMORY_MANAGER_HPP
#define MEMORY_MANAGER_HPP

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <ctime>
#include "common/cbasetypes.hpp"
#include "common/timer.hpp"

// Forward declarations
class MemoryEntry;
class MemoryBlock;

enum class MemoryType {
    SHORT_TERM,
    LONG_TERM,
    CONTEXTUAL
};

enum class MemoryPriority {
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL
};

struct MemoryMetadata {
    time_t creation_time;
    time_t last_access;
    time_t expiration_time;
    MemoryType type;
    MemoryPriority priority;
    size_t access_count;
    float importance_score;
};

class MemoryEntry {
private:
    int64_t id;
    std::string agent_id;
    std::string category;
    std::string data;
    MemoryMetadata metadata;
    
public:
    MemoryEntry(const std::string& agent_id, const std::string& category, 
                const std::string& data, MemoryType type, MemoryPriority priority);
    
    // Getters
    int64_t getId() const { return id; }
    const std::string& getAgentId() const { return agent_id; }
    const std::string& getCategory() const { return category; }
    const std::string& getData() const { return data; }
    const MemoryMetadata& getMetadata() const { return metadata; }
    
    // Memory management
    void access();
    void updateImportance(float new_score);
    bool isExpired(time_t current_time) const;
    float calculateRelevance(const std::string& context) const;
};

class MemoryBlock {
private:
    MemoryType type;
    size_t capacity;
    std::map<int64_t, std::shared_ptr<MemoryEntry>> entries;
    time_t retention_period;
    
public:
    MemoryBlock(MemoryType type, size_t capacity, time_t retention);
    
    // Memory operations
    bool addEntry(std::shared_ptr<MemoryEntry> entry);
    bool removeEntry(int64_t entry_id);
    std::shared_ptr<MemoryEntry> getEntry(int64_t entry_id);
    std::vector<std::shared_ptr<MemoryEntry>> getEntries(const std::string& category);
    
    // Maintenance
    void cleanup(time_t current_time);
    size_t getCurrentSize() const { return entries.size(); }
    bool isFull() const { return entries.size() >= capacity; }
};

class MemoryManager {
private:
    // Memory blocks
    std::unique_ptr<MemoryBlock> short_term_memory;
    std::unique_ptr<MemoryBlock> long_term_memory;
    std::map<std::string, std::unique_ptr<MemoryBlock>> contextual_memories;
    
    // Configuration
    size_t short_term_capacity;
    size_t long_term_capacity;
    time_t short_term_retention;
    time_t long_term_retention;
    
    // Memory management
    int64_t next_memory_id;
    time_t last_cleanup;
    int cleanup_interval;
    
    // Internal methods
    void consolidateMemories();
    void transferMemories(MemoryBlock& source, MemoryBlock& target);
    bool shouldTransferMemory(const MemoryEntry& entry);
    float calculateMemoryImportance(const MemoryEntry& entry);
    
public:
    MemoryManager(size_t short_term_cap, size_t long_term_cap,
                 time_t short_term_ret, time_t long_term_ret);
    ~MemoryManager();
    
    // Initialization
    bool initialize();
    void shutdown();
    
    // Core operations
    int64_t storeMemory(const std::string& agent_id, const std::string& category,
                        const std::string& data, MemoryType type, MemoryPriority priority);
    bool removeMemory(int64_t memory_id);
    std::shared_ptr<MemoryEntry> getMemory(int64_t memory_id);
    
    // Memory queries
    std::vector<std::shared_ptr<MemoryEntry>> queryMemories(
        const std::string& agent_id,
        const std::string& category,
        time_t start_time,
        time_t end_time
    );
    
    std::vector<std::shared_ptr<MemoryEntry>> findSimilarMemories(
        const std::string& context,
        const std::string& agent_id,
        size_t limit
    );
    
    // Context management
    void createContextualMemory(const std::string& context_id);
    void removeContextualMemory(const std::string& context_id);
    bool storeContextualMemory(const std::string& context_id, 
                             const std::string& agent_id,
                             const std::string& data);
    
    // Maintenance
    void update(time_t current_time);
    void cleanup();
    size_t getCurrentUsage() const;
    
    // Statistics
    struct MemoryStats {
        size_t short_term_count;
        size_t long_term_count;
        size_t contextual_count;
        size_t total_size;
        float consolidation_ratio;
        time_t oldest_memory;
        time_t newest_memory;
    };
    MemoryStats getStats() const;
};

#endif // MEMORY_MANAGER_HPP