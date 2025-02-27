#include "ai_beggar_agent.hpp"
#include <algorithm>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace rathena {
namespace ai {

AIBeggarAgent::AIBeggarAgent(const ConfigMap& config, std::shared_ptr<AIMemory> memory)
    : config_(config), memory_(memory), beggarCache_(), storyCache_(), questCache_(), conversationCache_() {
}

AIBeggarAgent::~AIBeggarAgent() {
    // Clean up resources
}

bool AIBeggarAgent::Initialize() {
    // Load beggar data from the database
    if (!LoadBeggarData()) {
        return false;
    }
    
    return true;
}

AIResponse AIBeggarAgent::ProcessEvent(const AIEvent& event) {
    AIResponse response;
    
    try {
        // Process the event based on its type
        switch (event.type) {
            case AIEventType::BEGGAR_TALK:
                {
                    int charId = std::stoi(event.data.at("char_id"));
                    int beggarId = std::stoi(event.data.at("beggar_id"));
                    std::string message = event.data.at("message");
                    
                    std::string beggarResponse = TalkToBeggar(charId, beggarId, message);
                    
                    response.success = true;
                    response.data["response"] = beggarResponse;
                }
                break;
                
            case AIEventType::BEGGAR_QUEST_ACCEPT:
                {
                    int charId = std::stoi(event.data.at("char_id"));
                    int beggarId = std::stoi(event.data.at("beggar_id"));
                    int questId = std::stoi(event.data.at("quest_id"));
                    
                    bool success = AcceptBeggarQuest(charId, beggarId, questId);
                    
                    response.success = success;
                    if (success) {
                        response.data["message"] = "Quest accepted successfully";
                    } else {
                        response.data["message"] = "Failed to accept quest";
                    }
                }
                break;
                
            case AIEventType::BEGGAR_QUEST_COMPLETE:
                {
                    int charId = std::stoi(event.data.at("char_id"));
                    int beggarId = std::stoi(event.data.at("beggar_id"));
                    int questId = std::stoi(event.data.at("quest_id"));
                    
                    bool success = CompleteBeggarQuest(charId, beggarId, questId);
                    
                    response.success = success;
                    if (success) {
                        response.data["message"] = "Quest completed successfully";
                    } else {
                        response.data["message"] = "Failed to complete quest";
                    }
                }
                break;
                
            case AIEventType::BEGGAR_GIVE_ITEM:
                {
                    int charId = std::stoi(event.data.at("char_id"));
                    int beggarId = std::stoi(event.data.at("beggar_id"));
                    int itemId = std::stoi(event.data.at("item_id"));
                    int amount = std::stoi(event.data.at("amount"));
                    
                    std::string beggarResponse = GiveItemToBeggar(charId, beggarId, itemId, amount);
                    
                    response.success = true;
                    response.data["response"] = beggarResponse;
                }
                break;
                
            case AIEventType::BEGGAR_GIVE_ZENY:
                {
                    int charId = std::stoi(event.data.at("char_id"));
                    int beggarId = std::stoi(event.data.at("beggar_id"));
                    int amount = std::stoi(event.data.at("amount"));
                    
                    std::string beggarResponse = GiveZenyToBeggar(charId, beggarId, amount);
                    
                    response.success = true;
                    response.data["response"] = beggarResponse;
                }
                break;
                
            case AIEventType::BEGGAR_CREATE:
                {
                    int mapId = std::stoi(event.data.at("map_id"));
                    int x = std::stoi(event.data.at("x"));
                    int y = std::stoi(event.data.at("y"));
                    
                    int beggarId = CreateBeggar(mapId, x, y);
                    
                    response.success = (beggarId > 0);
                    response.data["beggar_id"] = std::to_string(beggarId);
                    if (response.success) {
                        response.data["message"] = "Beggar created successfully";
                    } else {
                        response.data["message"] = "Failed to create beggar";
                    }
                }
                break;
                
            case AIEventType::BEGGAR_DELETE:
                {
                    int beggarId = std::stoi(event.data.at("beggar_id"));
                    
                    bool success = DeleteBeggar(beggarId);
                    
                    response.success = success;
                    if (success) {
                        response.data["message"] = "Beggar deleted successfully";
                    } else {
                        response.data["message"] = "Failed to delete beggar";
                    }
                }
                break;
                
            case AIEventType::BEGGAR_MOVE:
                {
                    int beggarId = std::stoi(event.data.at("beggar_id"));
                    int mapId = std::stoi(event.data.at("map_id"));
                    int x = std::stoi(event.data.at("x"));
                    int y = std::stoi(event.data.at("y"));
                    
                    bool success = MoveBeggar(beggarId, mapId, x, y);
                    
                    response.success = success;
                    if (success) {
                        response.data["message"] = "Beggar moved successfully";
                    } else {
                        response.data["message"] = "Failed to move beggar";
                    }
                }
                break;
                
            case AIEventType::BEGGAR_UPDATE:
                {
                    int beggarId = std::stoi(event.data.at("beggar_id"));
                    
                    // Extract beggar data from the event
                    std::map<std::string, std::string> beggarData;
                    for (const auto& pair : event.data) {
                        if (pair.first != "beggar_id" && pair.first != "type") {
                            beggarData[pair.first] = pair.second;
                        }
                    }
                    
                    bool success = UpdateBeggar(beggarId, beggarData);
                    
                    response.success = success;
                    if (success) {
                        response.data["message"] = "Beggar updated successfully";
                    } else {
                        response.data["message"] = "Failed to update beggar";
                    }
                }
                break;
                
            default:
                response.success = false;
                response.data["message"] = "Unsupported event type";
                break;
        }
    } catch (const std::exception& e) {
        response.success = false;
        response.data["message"] = "Error processing event: " + std::string(e.what());
    } catch (...) {
        response.success = false;
        response.data["message"] = "Unknown error processing event";
    }
    
    return response;
}

AIAgentType AIBeggarAgent::GetType() const {
    return AIAgentType::BEGGAR;
}

std::string AIBeggarAgent::GetName() const {
    return "AIBeggarAgent";
}

AIAgentStatus AIBeggarAgent::GetStatus() const {
    return AIAgentStatus::ACTIVE;
}

bool AIBeggarAgent::LoadBeggarData() {
    try {
        // In a real implementation, this would load data from the database
        // For now, we'll just create some sample data
        
        // Clear existing cache
        std::lock_guard<std::mutex> beggarLock(beggarCacheMutex_);
        beggarCache_.clear();
        
        // Create sample beggars
        for (int i = 1; i <= 5; ++i) {
            std::map<std::string, std::string> beggarData;
            beggarData["id"] = std::to_string(i);
            beggarData["name"] = "Beggar " + std::to_string(i);
            beggarData["map_id"] = "1";  // Prontera
            beggarData["x"] = std::to_string(150 + i * 10);
            beggarData["y"] = std::to_string(150 + i * 5);
            beggarData["appearance"] = "BEGGAR_APPEARANCE_" + std::to_string(i);
            beggarData["personality"] = "BEGGAR_PERSONALITY_" + std::to_string(i);
            beggarData["background"] = "BEGGAR_BACKGROUND_" + std::to_string(i);
            
            beggarCache_[i] = beggarData;
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log error
        return false;
    } catch (...) {
        // Log error
        return false;
    }
}

std::string AIBeggarAgent::GenerateBeggarStory(int beggarId, const std::map<std::string, std::string>& beggarData) {
    try {
        // Check if story is already cached
        {
            std::lock_guard<std::mutex> lock(storyCacheMutex_);
            auto it = storyCache_.find(beggarId);
            if (it != storyCache_.end()) {
                return it->second;
            }
        }
        
        // In a real implementation, this would use an AI provider to generate a story
        // For now, we'll just create a sample story
        std::string story = "The tale of " + beggarData.at("name") + " is a sad one. ";
        story += "Once a prosperous merchant, " + beggarData.at("name") + " lost everything in a terrible fire. ";
        story += "Now, " + beggarData.at("name") + " wanders the streets, hoping for kindness from strangers. ";
        story += "Despite the hardships, " + beggarData.at("name") + " maintains a " + beggarData.at("personality") + " attitude. ";
        story += "Perhaps one day, with enough help, " + beggarData.at("name") + " will be able to start anew.";
        
        // Cache the story
        {
            std::lock_guard<std::mutex> lock(storyCacheMutex_);
            storyCache_[beggarId] = story;
        }
        
        return story;
    } catch (const std::exception& e) {
        // Log error
        return "Error generating beggar story: " + std::string(e.what());
    } catch (...) {
        // Log error
        return "Unknown error generating beggar story";
    }
}

std::map<std::string, std::string> AIBeggarAgent::GenerateBeggarQuest(int beggarId, int charId, 
                                                                     const std::map<std::string, std::string>& beggarData,
                                                                     const std::map<std::string, std::string>& characterData) {
    try {
        // In a real implementation, this would use an AI provider to generate a quest
        // For now, we'll just create a sample quest
        std::map<std::string, std::string> questData;
        
        questData["id"] = std::to_string(beggarId * 100 + charId % 100);
        questData["beggar_id"] = std::to_string(beggarId);
        questData["char_id"] = std::to_string(charId);
        questData["title"] = "Help " + beggarData.at("name");
        questData["description"] = beggarData.at("name") + " needs your help. Bring 5 Healing Potions to ease their suffering.";
        questData["required_items"] = "501:5";  // 5 Healing Potions
        questData["reward_items"] = "512:1";    // 1 Blessing Scroll
        questData["reward_exp"] = "1000";
        questData["reward_zeny"] = "500";
        questData["status"] = "available";
        
        return questData;
    } catch (const std::exception& e) {
        // Log error
        std::map<std::string, std::string> errorData;
        errorData["error"] = "Error generating beggar quest: " + std::string(e.what());
        return errorData;
    } catch (...) {
        // Log error
        std::map<std::string, std::string> errorData;
        errorData["error"] = "Unknown error generating beggar quest";
        return errorData;
    }
}

std::string AIBeggarAgent::ProcessConversation(int beggarId, int charId, const std::string& message,
                                             const std::map<std::string, std::string>& beggarData,
                                             const std::map<std::string, std::string>& characterData) {
    try {
        // Get conversation history
        std::vector<std::pair<std::string, std::string>> history = GetConversationHistory(charId, beggarId);
        
        // In a real implementation, this would use an AI provider to generate a response
        // For now, we'll just create a sample response based on the message
        std::string response;
        
        if (message.find("hello") != std::string::npos || message.find("hi") != std::string::npos) {
            response = "Hello there, kind traveler. My name is " + beggarData.at("name") + ". Could you spare some change?";
        } else if (message.find("help") != std::string::npos) {
            response = "Oh, bless you for offering to help! I need some healing potions for my aching body. Could you bring me some?";
        } else if (message.find("story") != std::string::npos || message.find("background") != std::string::npos) {
            response = GenerateBeggarStory(beggarId, beggarData);
        } else if (message.find("quest") != std::string::npos) {
            response = "I do have a small favor to ask. Could you bring me 5 Healing Potions? I'll reward you with a Blessing Scroll.";
        } else if (message.find("thank") != std::string::npos) {
            response = "No, thank YOU for your kindness. It's rare to find such generosity these days.";
        } else {
            response = "I'm sorry, I don't understand. Could you please rephrase that?";
        }
        
        // Add to conversation history
        AddConversationEntry(charId, beggarId, message, response);
        
        return response;
    } catch (const std::exception& e) {
        // Log error
        return "Error processing conversation: " + std::string(e.what());
    } catch (...) {
        // Log error
        return "Unknown error processing conversation";
    }
}

std::vector<std::pair<std::string, std::string>> AIBeggarAgent::GetConversationHistory(int charId, int beggarId) {
    std::lock_guard<std::mutex> lock(conversationCacheMutex_);
    
    auto key = std::make_pair(charId, beggarId);
    auto it = conversationCache_.find(key);
    
    if (it != conversationCache_.end()) {
        return it->second;
    }
    
    return {};
}

void AIBeggarAgent::AddConversationEntry(int charId, int beggarId, const std::string& message, const std::string& response) {
    std::lock_guard<std::mutex> lock(conversationCacheMutex_);
    
    auto key = std::make_pair(charId, beggarId);
    conversationCache_[key].push_back(std::make_pair(message, response));
    
    // Limit history size
    const size_t maxHistorySize = 10;
    if (conversationCache_[key].size() > maxHistorySize) {
        conversationCache_[key].erase(conversationCache_[key].begin());
    }
}

bool AIBeggarAgent::HasCompletedQuest(int charId, int beggarId, int questId) {
    // In a real implementation, this would check the database
    // For now, we'll just return false
    return false;
}

bool AIBeggarAgent::HasRequiredItems(int charId, const std::map<std::string, std::string>& questData) {
    // In a real implementation, this would check the character's inventory
    // For now, we'll just return true
    return true;
}

bool AIBeggarAgent::RemoveRequiredItems(int charId, const std::map<std::string, std::string>& questData) {
    // In a real implementation, this would remove items from the character's inventory
    // For now, we'll just return true
    return true;
}

bool AIBeggarAgent::GiveRewards(int charId, const std::map<std::string, std::string>& questData) {
    // In a real implementation, this would add rewards to the character
    // For now, we'll just return true
    return true;
}

std::map<std::string, std::string> AIBeggarAgent::GetCharacterData(int charId) {
    // In a real implementation, this would load data from the database
    // For now, we'll just create some sample data
    std::map<std::string, std::string> characterData;
    
    characterData["id"] = std::to_string(charId);
    characterData["name"] = "Character " + std::to_string(charId);
    characterData["level"] = "50";
    characterData["job"] = "Swordsman";
    characterData["zeny"] = "10000";
    
    return characterData;
}

std::map<std::string, std::string> AIBeggarAgent::GetBeggarData(int beggarId) {
    std::lock_guard<std::mutex> lock(beggarCacheMutex_);
    
    auto it = beggarCache_.find(beggarId);
    if (it != beggarCache_.end()) {
        return it->second;
    }
    
    return {};
}

std::map<std::string, std::string> AIBeggarAgent::GetQuestData(int questId) {
    // In a real implementation, this would load data from the database
    // For now, we'll just create some sample data
    std::map<std::string, std::string> questData;
    
    questData["id"] = std::to_string(questId);
    questData["beggar_id"] = std::to_string(questId / 100);
    questData["char_id"] = std::to_string(questId % 100);
    questData["title"] = "Help Beggar " + std::to_string(questId / 100);
    questData["description"] = "Bring 5 Healing Potions to Beggar " + std::to_string(questId / 100);
    questData["required_items"] = "501:5";  // 5 Healing Potions
    questData["reward_items"] = "512:1";    // 1 Blessing Scroll
    questData["reward_exp"] = "1000";
    questData["reward_zeny"] = "500";
    questData["status"] = "available";
    
    return questData;
}

std::map<std::string, std::string> AIBeggarAgent::GenerateRandomPersonality() {
    // In a real implementation, this would generate a random personality
    // For now, we'll just create a sample personality
    std::map<std::string, std::string> personality;
    
    // Generate random personality traits
    std::vector<std::string> traits = {"kind", "bitter", "hopeful", "cynical", "friendly", "suspicious"};
    std::vector<std::string> backgrounds = {"merchant", "soldier", "noble", "farmer", "craftsman", "scholar"};
    
    // Seed random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Select random traits
    std::uniform_int_distribution<> traitDist(0, traits.size() - 1);
    std::uniform_int_distribution<> backgroundDist(0, backgrounds.size() - 1);
    
    personality["trait"] = traits[traitDist(gen)];
    personality["background"] = backgrounds[backgroundDist(gen)];
    
    return personality;
}

std::map<std::string, std::string> AIBeggarAgent::GenerateRandomAppearance(const std::map<std::string, std::string>& personality) {
    // In a real implementation, this would generate a random appearance based on personality
    // For now, we'll just create a sample appearance
    std::map<std::string, std::string> appearance;
    
    // Generate random appearance
    std::vector<std::string> clothes = {"tattered", "patched", "dirty", "faded", "worn"};
    std::vector<std::string> accessories = {"walking stick", "old hat", "bandage", "eye patch", "sack"};
    
    // Seed random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Select random appearance
    std::uniform_int_distribution<> clothesDist(0, clothes.size() - 1);
    std::uniform_int_distribution<> accessoriesDist(0, accessories.size() - 1);
    
    appearance["clothes"] = clothes[clothesDist(gen)];
    appearance["accessory"] = accessories[accessoriesDist(gen)];
    
    return appearance;
}

std::map<std::string, std::string> AIBeggarAgent::GenerateRandomLocation() {
    // In a real implementation, this would generate a random location
    // For now, we'll just create a sample location
    std::map<std::string, std::string> location;
    
    // Generate random location
    std::vector<std::string> maps = {"prontera", "geffen", "payon", "morroc", "alberta"};
    
    // Seed random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Select random map
    std::uniform_int_distribution<> mapDist(0, maps.size() - 1);
    std::uniform_int_distribution<> coordDist(100, 200);
    
    location["map"] = maps[mapDist(gen)];
    location["x"] = std::to_string(coordDist(gen));
    location["y"] = std::to_string(coordDist(gen));
    
    return location;
}

int AIBeggarAgent::CreateBeggar(int mapId, int x, int y) {
    try {
        // Generate a new beggar ID
        int beggarId = 0;
        
        {
            std::lock_guard<std::mutex> lock(beggarCacheMutex_);
            
            // Find the highest existing beggar ID
            for (const auto& pair : beggarCache_) {
                beggarId = std::max(beggarId, pair.first);
            }
            
            // Increment for the new beggar
            beggarId++;
        }
        
        // Generate random personality and appearance
        std::map<std::string, std::string> personality = GenerateRandomPersonality();
        std::map<std::string, std::string> appearance = GenerateRandomAppearance(personality);
        
        // Create beggar data
        std::map<std::string, std::string> beggarData;
        beggarData["id"] = std::to_string(beggarId);
        beggarData["name"] = "Beggar " + std::to_string(beggarId);
        beggarData["map_id"] = std::to_string(mapId);
        beggarData["x"] = std::to_string(x);
        beggarData["y"] = std::to_string(y);
        beggarData["appearance"] = appearance["clothes"] + " clothes with " + appearance["accessory"];
        beggarData["personality"] = personality["trait"];
        beggarData["background"] = personality["background"];
        
        // Add to cache
        {
            std::lock_guard<std::mutex> lock(beggarCacheMutex_);
            beggarCache_[beggarId] = beggarData;
        }
        
        // In a real implementation, this would also save to the database
        
        return beggarId;
    } catch (const std::exception& e) {
        // Log error
        return 0;
    } catch (...) {
        // Log error
        return 0;
    }
}

bool AIBeggarAgent::DeleteBeggar(int beggarId) {
    try {
        // Remove from cache
        {
            std::lock_guard<std::mutex> lock(beggarCacheMutex_);
            auto it = beggarCache_.find(beggarId);
            if (it == beggarCache_.end()) {
                return false;
            }
            
            beggarCache_.erase(it);
        }
        
        // Remove from story cache
        {
            std::lock_guard<std::mutex> lock(storyCacheMutex_);
            auto it = storyCache_.find(beggarId);
            if (it != storyCache_.end()) {
                storyCache_.erase(it);
            }
        }
        
        // Remove from quest cache
        {
            std::lock_guard<std::mutex> lock(questCacheMutex_);
            auto it = questCache_.find(beggarId);
            if (it != questCache_.end()) {
                questCache_.erase(it);
            }
        }
        
        // In a real implementation, this would also delete from the database
        
        return true;
    } catch (const std::exception& e) {
        // Log error
        return false;
    } catch (...) {
        // Log error
        return false;
    }
}

std::vector<std::map<std::string, std::string>> AIBeggarAgent::ListBeggars() {
    std::vector<std::map<std::string, std::string>> beggars;
    
    std::lock_guard<std::mutex> lock(beggarCacheMutex_);
    
    for (const auto& pair : beggarCache_) {
        beggars.push_back(pair.second);
    }
    
    return beggars;
}

std::map<std::string, std::string> AIBeggarAgent::GetBeggarInfo(int beggarId) {
    return GetBeggarData(beggarId);
}

std::string AIBeggarAgent::GetBeggarStory(int beggarId) {
    std::map<std::string, std::string> beggarData = GetBeggarData(beggarId);
    if (beggarData.empty()) {
        return "Beggar not found";
    }
    
    return GenerateBeggarStory(beggarId, beggarData);
}

std::vector<std::map<std::string, std::string>> AIBeggarAgent::ListBeggarQuests(int beggarId) {
    std::vector<std::map<std::string, std::string>> quests;
    
    // In a real implementation, this would load quests from the database
    // For now, we'll just create a sample quest
    std::map<std::string, std::string> questData;
    
    questData["id"] = std::to_string(beggarId * 100);
    questData["beggar_id"] = std::to_string(beggarId);
    questData["title"] = "Help Beggar " + std::to_string(beggarId);
    questData["description"] = "Bring 5 Healing Potions to Beggar " + std::to_string(beggarId);
    questData["required_items"] = "501:5";  // 5 Healing Potions
    questData["reward_items"] = "512:1";    // 1 Blessing Scroll
    questData["reward_exp"] = "1000";
    questData["reward_zeny"] = "500";
    questData["status"] = "available";
    
    quests.push_back(questData);
    
    return quests;
}

std::map<std::string, std::string> AIBeggarAgent::GetBeggarQuest(int beggarId, int questId) {
    // In a real implementation, this would load the quest from the database
    // For now, we'll just create a sample quest
    return GetQuestData(questId);
}

std::string AIBeggarAgent::TalkToBeggar(int charId, int beggarId, const std::string& message) {
    std::map<std::string, std::string> beggarData = GetBeggarData(beggarId);
    if (beggarData.empty()) {
        return "Beggar not found";
    }
    
    std::map<std::string, std::string> characterData = GetCharacterData(charId);
    
    return ProcessConversation(beggarId, charId, message, beggarData, characterData);
}

bool AIBeggarAgent::AcceptBeggarQuest(int charId, int beggarId, int questId) {
    // In a real implementation, this would update the database
    // For now, we'll just return true
    return true;
}

bool AIBeggarAgent::CompleteBeggarQuest(int charId, int beggarId, int questId) {
    try {
        // Get quest data
        std::map<std::string, std::string> questData = GetQuestData(questId);
        if (questData.empty()) {
            return false;
        }
        
        // Check if the character has the required items
        if (!HasRequiredItems(charId, questData)) {
            return false;
        }
        
        // Remove the required items
        if (!RemoveRequiredItems(charId, questData)) {
            return false;
        }
        
        // Give the rewards
        if (!GiveRewards(charId, questData)) {
            return false;
        }
        
        // Update quest status
        // In a real implementation, this would update the database
        
        return true;
    } catch (const std::exception& e) {
        // Log error
        return false;
    } catch (...) {
        // Log error
        return false;
    }
}

std::string AIBeggarAgent::GiveItemToBeggar(int charId, int beggarId, int itemId, int amount) {
    std::map<std::string, std::string> beggarData = GetBeggarData(beggarId);
    if (beggarData.empty()) {
        return "Beggar not found";
    }
    
    // In a real implementation, this would check if the character has the item
    // and generate an appropriate response based on the item
    
    // For now, we'll just create a sample response
    std::string response = "Thank you for the ";
    
    // Sample item names
    std::map<int, std::string> itemNames = {
        {501, "Healing Potion"},
        {502, "Mana Potion"},
        {503, "Apple"},
        {504, "Bread"},
        {505, "Meat"}
    };
    
    auto it = itemNames.find(itemId);
    if (it != itemNames.end()) {
        response += it->second;
    } else {
        response += "item";
    }
    
    if (amount > 1) {
        response += "s";
    }
    
    response += "! May the gods bless you for your kindness.";
    
    return response;
}

std::string AIBeggarAgent::GiveZenyToBeggar(int charId, int beggarId, int amount) {
    std::map<std::string, std::string> beggarData = GetBeggarData(beggarId);
    if (beggarData.empty()) {
        return "Beggar not found";
    }
    
    // In a real implementation, this would check if the character has the zeny
    // and generate an appropriate response based on the amount
    
    // For now, we'll just create a sample response
    std::string response;
    
    if (amount < 100) {
        response = "Thank you for the " + std::to_string(amount) + " zeny. Every little bit helps.";
    } else if (amount < 1000) {
        response = "Oh my! " + std::to_string(amount) + " zeny! You are most generous. Thank you!";
    } else {
        response = std::to_string(amount) + " zeny?! I... I don't know what to say. This will change my life. Bless you!";
    }
    
    return response;
}

bool AIBeggarAgent::MoveBeggar(int beggarId, int mapId, int x, int y) {
    try {
        std::lock_guard<std::mutex> lock(beggarCacheMutex_);
        
        auto it = beggarCache_.find(beggarId);
        if (it == beggarCache_.end()) {
            return false;
        }
        
        // Update location
        it->second["map_id"] = std::to_string(mapId);
        it->second["x"] = std::to_string(x);
        it->second["y"] = std::to_string(y);
        
        // In a real implementation, this would also update the database
        
        return true;
    } catch (const std::exception& e) {
        // Log error
        return false;
    } catch (...) {
        // Log error
        return false;
    }
}

bool AIBeggarAgent::UpdateBeggar(int beggarId, const std::map<std::string, std::string>& data) {
    try {
        std::lock_guard<std::mutex> lock(beggarCacheMutex_);
        
        auto it = beggarCache_.find(beggarId);
        if (it == beggarCache_.end()) {
            return false;
        }
        
        // Update data
        for (const auto& pair : data) {
            it->second[pair.first] = pair.second;
        }
        
        // In a real implementation, this would also update the database
        
        return true;
    } catch (const std::exception& e) {
        // Log error
        return false;
    } catch (...) {
        // Log error
        return false;
    }
}

} // namespace ai
} // namespace rathena
