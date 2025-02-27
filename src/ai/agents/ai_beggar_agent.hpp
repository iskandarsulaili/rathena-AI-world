#ifndef AI_BEGGAR_AGENT_HPP
#define AI_BEGGAR_AGENT_HPP

#include "../common/ai_agent_base.hpp"
#include "../common/ai_types.hpp"
#include "../common/ai_request.hpp"
#include "../common/ai_response.hpp"
#include "../common/ai_config.hpp"

namespace rathena {
struct block_list;
struct npc_data;
}

namespace ai {

class AIBeggarAgent : public AIAgentBase {
public:
    explicit AIBeggarAgent(const AIConfig& config);
    ~AIBeggarAgent() override;

    // Visual state enumeration
    enum class VisualState {
        NORMAL,      // Default tattered appearance
        TRADING,     // Trading with player
        SPECIAL      // Full power form
    };

    // Base model configuration
    struct ModelConfig {
        int baseModelId = 4;          // High Priest model ID
        int staffItemId = 1619;       // Sage's Staff item ID
        int robeItemId = 2326;        // Modified High Priest robe
        int glowEffectId = 312;       // Gloria effect ID
        int specialEffectId = 688;    // Archbishop transform effect
    };

    // Visual effect configuration  
    struct EffectConfig {
        int normalAuraId = 313;       // Faded Gloria particles
        int tradeAuraId = 312;        // Full Gloria effect
        int specialAuraId = 689;      // Enhanced Archbishop aura
        int footstepEffectId = 314;   // Modified Gloria ground effect
        int runeEffectId = 492;       // Scripture effect
        int lightEffectId = 152;      // Divine light effect
    };

    // Animation configuration
    struct AnimationConfig {
        int normalWalkAnim = 0;       // Modified walk animation
        int normalIdleAnim = 1;       // Standard idle animation
        int tradeAnim = 4;           // Modified cast animation
        int specialAnim = 8;         // Battle stance animation
    };

    // Visual state management
    void setVisualState(VisualState state);
    VisualState getVisualState() const;
    void updateVisualEffects();

    // Model and effect getters
    const ModelConfig& getModelConfig() const;
    const EffectConfig& getEffectConfig() const;
    const AnimationConfig& getAnimationConfig() const;

    // Interaction handlers with visual effects
    void onPlayerApproach(int playerId);
    void onTradeStart(int playerId);
    void onTradeComplete(int playerId);
    void onSpecialEvent(int playerId);

    // AIAgentBase virtual function implementations
    void update() override;
    AIResponse processRequest(const AIRequest& request) override;
    AIRequest createRequest() override;

protected:
    // Visual state management
    void applyNormalState();
    void applyTradingState();
    void applySpecialState();

    // Effect utilities
    void playEffect(int effectId, bool persistent = false);
    void stopEffect(int effectId);
    void updateAnimation(int animId);
    void updateAura(int auraId);

private:
    VisualState currentState;
    ModelConfig modelConfig;
    EffectConfig effectConfig;
    AnimationConfig animConfig;

    // State tracking
    bool isTrading;
    bool isSpecialEvent;
    int currentAuraEffect;
    int currentAnimation;
    rathena::block_list* bl;  // rAthena block list pointer
};

} // namespace ai

#endif // AI_BEGGAR_AGENT_HPP