# AI Legends System

## Overview

The AI Legends system introduces dynamic bloodlines to rAthena, allowing players to unlock ancestral connections to powerful beings or forces. Each bloodline provides unique skills, stories, and progression paths that enhance gameplay.

This system uses AI to generate personalized content for each player, including bloodline stories, skill descriptions, and interactive conversations with ancestral spirits.

## Key Features

### Bloodlines
- Multiple unlockable bloodlines with unique attributes
- Separate progression system from character levels
- Specific unlock requirements (level, stats, quests, items)

### Skills
- Unique skills for each bloodline
- Skill progression tied to bloodline level
- Active abilities, passive bonuses, buffs, and debuffs
- Balanced cooldown system

### AI-Generated Content
- Personalized bloodline stories
- Dynamic skill descriptions
- Interactive conversations with bloodline spirits
- Custom level-up messages

### Events
- Random bloodline-related events
- Special challenges and trials
- Experience and item rewards

## Configuration

The AI Legends system is highly configurable through the `ai_legends.conf` file. Key configuration options include:

- Maximum number of bloodlines per character
- Experience requirements and curve
- Skill learning requirements
- AI provider settings
- Event frequency and rewards

## Database Structure

The system uses several tables:
- `ai_bloodlines`: Base bloodline definitions
- `ai_character_bloodlines`: Character bloodline progress
- `ai_bloodline_skills`: Skills for each bloodline
- `ai_character_bloodline_skills`: Character skill progress
- `ai_bloodline_stories`: Generated stories
- `ai_bloodline_conversations`: Conversation history

## Integration

The AI Legends system integrates with:
- Character system (stats, levels, jobs)
- Skill system (custom skills)
- Quest system (unlock requirements, special quests)
- Item system (requirements, rewards)
- Chat system (conversations)

## AI Providers

The system supports multiple AI providers:
- OpenAI
- Azure OpenAI
- DeepSeek
- Local LLM models

Each provider can be configured with different models and parameters for various content types.

## Commands

Players can use the following commands:
- `/bloodline list` - List available bloodlines
- `/bloodline info [id]` - Show bloodline information
- `/bloodline skills [id]` - Show bloodline skills
- `/bloodline story [id]` - Show bloodline story
- `/bloodline talk [id] [message]` - Talk to bloodline spirit

## Developer API

The AI Legends system provides an API for developers to:
- Register new bloodlines
- Create custom bloodline skills
- Trigger bloodline events
- Access bloodline data for characters
- Generate custom AI content

## Performance Considerations

- AI content generation is asynchronous to prevent lag
- Generated content is cached to reduce API calls
- Local fallback options when AI services are unavailable
- Configurable rate limits and token usage

## Future Enhancements

Planned enhancements include:
- Bloodline fusion system
- Family bloodline inheritance
- Guild bloodline bonuses
- PvP bloodline challenges
- Seasonal bloodline events