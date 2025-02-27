#include "ai_beggar_agent.hpp"
#include "../common/ai_response.hpp"
#include "../common/ai_request.hpp"

// rAthena includes
#include "../../map/clif.hpp"
#include "../../map/npc.hpp"
#include "../../map/pc.hpp"
#include "../../map/skill.hpp"
#include "../../map/unit.hpp"

namespace ai {

AIBeggarAgent::AIBeggarAgent(const AIConfig& config) : 
    AIAgentBase(config),
    currentState(VisualState::NORMAL),
    isTrading(false),
    isSpecialEvent(false),
    currentAuraEffect(0),
    currentAnimation(0) {
    
    // Create NPC entity
    struct npc_data* nd = npc_create_npc(config.mapId, config.x, config.y);
    bl = &nd->bl;
    
    // Initialize with High Priest model
    nd->class_ = modelConfig.baseModelId;
    nd->speed = 200; // Slow walking speed
    
    // Apply initial visual state
    applyNormalState();
}

AIBeggarAgent::~AIBeggarAgent() {
    if (bl) {
        // Clean up any persistent effects
        if (currentAuraEffect) {
            clif_clearunit_area(bl, CLR_OUTSIGHT);
        }
        unit_free(bl, CLR_DEAD);
    }
}

void AIBeggarAgent::setVisualState(VisualState state) {
    if (currentState != state) {
        currentState = state;
        updateVisualEffects();
    }
}

void AIBeggarAgent::updateVisualEffects() {
    switch (currentState) {
        case VisualState::NORMAL:
            applyNormalState();
            break;
        case VisualState::TRADING:
            applyTradingState();
            break;
        case VisualState::SPECIAL:
            applySpecialState();
            break;
    }
}

void AIBeggarAgent::applyNormalState() {
    // Basic tattered appearance
    updateAnimation(animConfig.normalIdleAnim);
    stopEffect(currentAuraEffect);
    currentAuraEffect = effectConfig.normalAuraId;
    playEffect(currentAuraEffect, true);
    
    // Set up basic equipment
    clif_changelook(bl, LOOK_WEAPON, modelConfig.staffItemId);
    clif_changelook(bl, LOOK_ROBE, modelConfig.robeItemId);
}

void AIBeggarAgent::applyTradingState() {
    // Trading state with glowing effects
    updateAnimation(animConfig.tradeAnim);
    stopEffect(currentAuraEffect);
    currentAuraEffect = effectConfig.tradeAuraId;
    playEffect(currentAuraEffect, true);
    playEffect(effectConfig.runeEffectId);
    playEffect(effectConfig.lightEffectId);
}

void AIBeggarAgent::applySpecialState() {
    // Full power form
    updateAnimation(animConfig.specialAnim);
    stopEffect(currentAuraEffect);
    currentAuraEffect = effectConfig.specialAuraId;
    playEffect(currentAuraEffect, true);
    playEffect(modelConfig.specialEffectId);
}

void AIBeggarAgent::onPlayerApproach(int playerId) {
    // Show subtle glow effect
    playEffect(effectConfig.normalAuraId);
}

void AIBeggarAgent::onTradeStart(int playerId) {
    isTrading = true;
    setVisualState(VisualState::TRADING);
}

void AIBeggarAgent::onTradeComplete(int playerId) {
    isTrading = false;
    setVisualState(VisualState::NORMAL);
    
    // Play reward effect
    playEffect(effectConfig.runeEffectId);
}

void AIBeggarAgent::playEffect(int effectId, bool persistent) {
    if (bl) {
        clif_specialeffect(bl, effectId, (persistent ? AREA : SELF));
    }
}

void AIBeggarAgent::stopEffect(int effectId) {
    if (bl && effectId) {
        clif_clearunit_single(effectId, bl);
    }
}

void AIBeggarAgent::updateAnimation(int animId) {
    if (bl && currentAnimation != animId) {
        currentAnimation = animId;
        clif_emotion(bl, static_cast<emotion_type>(animId));
    }
}

void AIBeggarAgent::update() {
    // Update visual effects based on state
    updateVisualEffects();
    
    // Handle footstep effects while moving
    if (bl && unit_is_walking(bl)) {
        playEffect(effectConfig.footstepEffectId);
    }
    
    // Update base agent behavior
    AIAgentBase::update();
}

AIResponse AIBeggarAgent::processRequest(const AIRequest& request) {
    AIResponse response = AIAgentBase::processRequest(request);
    
    // Handle any visual effects needed for AI responses
    if (response.shouldShowEmote && bl) {
        clif_emotion(bl, static_cast<emotion_type>(response.emoteId));
    }
    
    return response;
}

AIRequest AIBeggarAgent::createRequest() {
    AIRequest request = AIAgentBase::createRequest();
    
    // Add current visual state to context
    request.addContext("visual_state", static_cast<int>(currentState));
    request.addContext("is_trading", isTrading);
    request.addContext("is_special_event", isSpecialEvent);
    
    return request;
}

const AIBeggarAgent::ModelConfig& AIBeggarAgent::getModelConfig() const {
    return modelConfig;
}

const AIBeggarAgent::EffectConfig& AIBeggarAgent::getEffectConfig() const {
    return effectConfig;
}

const AIBeggarAgent::AnimationConfig& AIBeggarAgent::getAnimationConfig() const {
    return animConfig;
}

} // namespace ai
