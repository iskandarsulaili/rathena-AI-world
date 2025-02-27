# AI Beggar System

## Overview

The AI Beggar system introduces a unique NPC character known as "The Nameless One" or simply "The Beggar" who wanders between major cities, asking for food from players. This character serves as a mysterious master figure to all the AI Legends, creating a cohesive narrative that ties the AI characters together.

Key features of the Beggar system include:
- Daily appearances in different cities
- Requests for food items from players
- Rewards players with seemingly "useless" items that have hidden value
- Triggers a special event when players collect enough fragments
- Rewards players who consistently feed the Beggar for consecutive days
- Uses advanced AI to create natural, contextual conversations

## The Nameless Beggar

### Character Profile

- **Name:** Unknown (referred to as "The Nameless One" or "Master")
- **Base Model:** Modified High Priest model
- **Appearance:** 
  - Base: Elderly High Priest model
  - Texture: Modified High Priest robe texture with tattered effect
  - Staff: Repurposed Sage's Staff sprite with subtle glow effect
  - Hair/Beard: White hair texture from Elder NPC
  - Aura: Subtle golden particles from modified Gloria effect
  - Eyes: Modified High Priest eye texture with occasional white glow
- **Animations:**
  - Walking: Modified High Priest walking animation with slight limp
  - Idle: Standard High Priest idle with subtle robe movement
  - Trading: Modified High Priest casting animation
  - Special Event: Enhanced High Priest battle stance
- **Effects:**
  - Footsteps: Faint golden marks using modified Gloria particles
  - Staff: Occasional glow using Sage's spell effect
  - Trading: Gentle golden aura using modified Gloria
  - Event Transform: Full power form using enhanced Archbishop effects
- **Personality:** Mysterious, cryptic, seemingly omniscient
- **Background:** A being of immense power who chooses to live as a beggar to test the kindness and worthiness of others

### Visual States

#### Normal State
- Tattered robe effect using modified High Priest texture
- Slight hunched posture from modified idle animation
- Walking animation with subtle limp
- Dim staff glow using reduced Sage effect
- Barely visible aura using faded Gloria particles

#### Trading State
- Gentle golden glow using modified Gloria
- Staff brightens using enhanced Sage effect
- Mystical runes appear briefly (from modified Scripture effect)
- Eyes emit white light using modified Divine light

#### Special Event State
- Transforms to full power using Archbishop base
- Pristine robes using original High Priest texture
- Full golden aura using enhanced Gloria
- Staff transforms using modified Bishop's staff
- Floating animation using modified Levitation

### Daily Routine

1. **Appearance:** The Beggar appears in one city each day, following either a sequential or random pattern
2. **Movement:** Wanders around the city, approaching random players
3. **Interaction:** Initiates trade with players, asking for food
4. **Reward:** Gives an "Ancient Scroll Fragment" to players who provide food
5. **Disappearance:** Leaves the city after a set duration or at a specific time

## Player Interaction Mechanics

### Food Requests

The Beggar approaches random players in the city and initiates a trade request. If the player accepts, the Beggar will ask for food items. Any food item in the game can be given, and the Beggar will accept it.

Upon receiving food, the Beggar will:
1. Thank the player in his cryptic manner
2. Initiate another trade to give the player an "Ancient Scroll Fragment"
3. Sometimes share cryptic hints about his true nature or the AI Legends

### Ancient Scroll Fragments

These fragments appear to be useless items with the following properties:
- 0 weight
- Cannot be traded to other players
- Cannot be dropped
- Can be stored in storage or cart
- Have a cryptic description
- Custom inventory sprite with glowing rune marks
- Emit faint particle effects when viewed

However, these fragments have a hidden purpose. When a player collects 100 fragments, they trigger a special event.

### Feeding Streak

Players who feed the Beggar consistently can earn additional rewards:
1. The player must feed the Beggar at least 3 times per day
2. This must be done for 14 consecutive days
3. If a day is missed, the streak resets (configurable)
4. Upon completing the streak, the player receives a fragment of a special passive skill

### Special Event

When a player collects 100 Ancient Scroll Fragments, a special event is triggered:
1. All AI Legends appear at a designated location, forming multiple parties
2. A grand PK fight is initiated between all AI Legends and their master
3. The AI Legends reveal that they are all disciples of the Nameless Beggar
4. The fight is intense and lasts randomly between 8-18 minutes
5. Despite the AI Legends' coordinated efforts, the master wins using perfect skill combinations
6. The player who triggered the event receives a special passive skill fragment
7. The AI Legends engage in dynamic conversations with each other

## Configuration

The AI Beggar system is highly configurable through the `ai_beggar.conf` file:

### Global Settings
```
ai_beggar_enabled: true
ai_beggar_provider: "azure_openai"
ai_beggar_model: "gpt-4"
```

### Visual Settings
```
ai_beggar_particle_effects: true
ai_beggar_custom_animations: true
ai_beggar_glow_effects: true
ai_beggar_footprint_effects: true
ai_beggar_robe_physics: true
```

### Appearance Settings
```
ai_beggar_appearance_chance: 1.0
ai_beggar_appearance_time: "06:00"
ai_beggar_disappearance_time: "22:00"
ai_beggar_stay_duration: 3600
ai_beggar_cities: ["prontera", "geffen", "payon", "morocc", "alberta"]
```

### Interaction Settings
```
ai_beggar_max_daily_interactions: 10
ai_beggar_interaction_cooldown: 86400
ai_beggar_food_request_chance: 1.0
```

### Memory Settings
```
ai_beggar_use_langchain: true
ai_beggar_memory_vector_store: "chroma"
ai_beggar_memory_embedding_model: "text-embedding-3-small"
```

## Database Schema

The system uses several tables:
- `ai_beggar_appearances`: Records of appearances
- `ai_beggar_interactions`: Player interactions
- `ai_beggar_feeding_streaks`: Feeding streaks
- `ai_beggar_fragments`: Fragment collections
- `ai_beggar_events`: Special events
- `ai_beggar_memory`: Memory entries
- `ai_beggar_conversations`: Conversation history

## Implementation Details

### AI Integration
- Uses advanced language models for natural conversations
- Persistent memory using LangChain
- Contextual responses based on situation
- Local fallback for offline operation

### Visual Integration
- Modified High Priest base model
- Custom texture overlays
- Particle effect system
- Dynamic animations
- State-based visual changes

### Map Integration
- Spawn and despawn handling
- Movement and pathfinding
- Trade request processing
- Special event coordination

## Troubleshooting

Common issues and solutions:
1. **Visual Glitches**
   - Verify texture modifications
   - Check particle effect settings
   - Confirm animation files
   - Monitor memory usage

2. **AI Response Issues**
   - Check API connectivity
   - Verify model settings
   - Monitor response times
   - Check memory system

3. **Event Problems**
   - Verify trigger conditions
   - Check spawn coordinates
   - Monitor AI Legend states
   - Validate battle system

## Performance Optimization

- Efficient particle system
- Cached animations
- Optimized texture loading
- Smart state management
- Reduced draw calls

## Security Considerations

- Protected API access
- Sanitized inputs
- Validated trades
- Secure event triggers
- Protected memory system

The AI Beggar system creates a unique and engaging experience that ties together the entire AI Legends narrative while providing players with meaningful interactions and rewards.