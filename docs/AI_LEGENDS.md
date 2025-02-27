# AI Legends System

## Overview

The AI Legends system introduces dynamic bloodlines to rAthena, allowing players to unlock ancestral connections to powerful beings or forces. Each bloodline provides unique skills, stories, and progression paths that enhance gameplay.

This system uses AI to generate personalized content for each player, including bloodline stories, skill descriptions, and interactive conversations with ancestral spirits. For detailed information about each bloodline, including their visual appearances and sprite/model details, see [AI_LEGENDS_PROFILES.md](./AI_LEGENDS_PROFILES.md).

## Key Features

### Bloodlines
- Multiple unlockable bloodlines with unique attributes
- Separate progression system from character levels
- Specific unlock requirements (level, stats, quests, items)
- Unique visual effects and custom animations
- Dynamic character transformations

### Skills
- Unique skills for each bloodline
- Skill progression tied to bloodline level
- Active abilities, passive bonuses, buffs, and debuffs
- Balanced cooldown system
- Custom skill animations and particles

### AI-Generated Content
- Personalized bloodline stories
- Dynamic skill descriptions
- Interactive conversations with bloodline spirits
- Custom level-up messages
- Contextual combat dialogue

### Events
- Random bloodline-related events
- Special challenges and trials
- Experience and item rewards
- Multi-bloodline battles
- Special crossover events with The Nameless Beggar (see [AI_BEGGAR.md](./AI_BEGGAR.md))

## Model Integration

### Character Models
- Each bloodline uses modified advanced class models
- Custom texture overlays for visual effects
- Dynamic particle systems
- Unique animation sets
- Real-time model transformations

### Visual Effects
- Custom particle effects for each bloodline
- Environment interactions (footsteps, auras)
- Weather effects during special abilities
- Dynamic lighting integration
- Skill-specific animations

### Animation System
- Smooth transitions between states
- Blended animations for natural movement
- Transform animations for special abilities
- Custom idle and combat stances
- Emotional expression animations

## Configuration

The AI Legends system is highly configurable through the `ai_legends.conf` file. Key configuration options include:

### Core Settings
```
ai_legends_enabled: true
ai_legends_provider: "azure_openai"
ai_legends_model: "gpt-4"
ai_legends_max_bloodlines: 3
```

### Visual Settings
```
ai_legends_particle_effects: true
ai_legends_custom_animations: true
ai_legends_environment_effects: true
ai_legends_transform_effects: true
```

### Progression Settings
```
ai_legends_exp_curve: "medium"
ai_legends_skill_points: 5
ai_legends_max_level: 100
```

### Event Settings
```
ai_legends_event_frequency: 3600
ai_legends_special_event_chance: 0.1
ai_legends_crossover_events: true
```

## Database Structure

The system uses several tables:
- `ai_bloodlines`: Base bloodline definitions
- `ai_character_bloodlines`: Character bloodline progress
- `ai_bloodline_skills`: Skills for each bloodline
- `ai_character_bloodline_skills`: Character skill progress
- `ai_bloodline_stories`: Generated stories
- `ai_bloodline_conversations`: Conversation history
- `ai_visual_effects`: Visual effect configurations
- `ai_animation_sets`: Custom animation definitions

## Integration

The AI Legends system integrates with:
- Character system (stats, levels, jobs)
- Skill system (custom skills)
- Quest system (unlock requirements, special quests)
- Item system (requirements, rewards)
- Chat system (conversations)
- Visual system (effects, animations)
- The Nameless Beggar system

## AI Providers

The system supports multiple AI providers:
- OpenAI
- Azure OpenAI
- DeepSeek
- Local LLM models

Each provider can be configured with different models and parameters for various content types:
```
ai_legend_story_model: "gpt-4"
ai_legend_conversation_model: "gpt-3.5-turbo"
ai_legend_skill_description_model: "gpt-3.5-turbo"
```

## Commands

Players can use the following commands:
- `/bloodline list` - List available bloodlines
- `/bloodline info [id]` - Show bloodline information
- `/bloodline skills [id]` - Show bloodline skills
- `/bloodline story [id]` - Show bloodline story
- `/bloodline talk [id] [message]` - Talk to bloodline spirit
- `/bloodline transform [id]` - Transform into bloodline form
- `/bloodline effects [id] [on/off]` - Toggle visual effects

## Developer API

The AI Legends system provides an API for developers to:
- Register new bloodlines
- Create custom bloodline skills
- Trigger bloodline events
- Access bloodline data for characters
- Generate custom AI content
- Add custom visual effects
- Define new animations

### Example: Adding a New Bloodline
```cpp
AILegends::registerBloodline({
    .id = "celestial_dragon",
    .name = "Celestial Dragon",
    .element = ELEMENT_HOLY | ELEMENT_FIRE,
    .baseModel = "HighWizard",
    .transformModel = "CelestialDragon",
    .particleEffects = {"holy_aura", "dragon_flames"},
    .animations = {"dragon_idle", "dragon_cast", "dragon_transform"}
});
```

## Performance Considerations

- AI content generation is asynchronous to prevent lag
- Generated content is cached to reduce API calls
- Local fallback options when AI services are unavailable
- Configurable rate limits and token usage
- Efficient particle system pooling
- Animation state caching
- LOD system for visual effects
- Batch processing for skill effects

## Future Enhancements

Planned enhancements include:
- Bloodline fusion system
- Family bloodline inheritance
- Guild bloodline bonuses
- PvP bloodline challenges
- Seasonal bloodline events
- Enhanced transformation effects
- Advanced particle systems
- Weather integration
- Day/night cycle effects

## Cross-System Integration

The AI Legends system is tightly integrated with other major systems:

### The Nameless Beggar
- Special events where the Beggar reveals bloodline secrets
- Unique rewards for bloodline holders
- Story connections between bloodlines and the Beggar
- Combined visual effects during special events

### Map System
- Environmental effects based on bloodline
- Special bloodline-only areas
- Dynamic weather changes during events
- Custom map effects for skills

### Guild System
- Guild bloodline bonuses
- Special guild events
- Shared bloodline benefits
- Guild transformation events