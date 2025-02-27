# Client Updates for AI Integration

This document outlines the required client-side updates to support the AI features in rAthena AI World.

## 1. GRF File Updates

### data.grf Updates
- **Sprite Updates**
  - Add new sprite files for each AI Legend character (4060-4095 class IDs)
  - Add new effect sprites for special AI skills (8001-8036 skill IDs)
  - Add new map sprites for AI interaction points

- **Effect Files**
  - Add new effects for AI Legend unique abilities
  - Add special effects for secret skill reveals
  - Add environmental effects for AI-influenced weather changes

- **Sound Files**
  - Add voice lines for AI Legend interactions
  - Add special sound effects for AI abilities
  - Add ambient sounds for AI-influenced areas

### rdata.grf Updates
- **Map Updates**
  - Add new map files for AI Legend spawn locations
  - Update existing maps with AI interaction points
  - Add pathfinding data for AI movement areas

- **Item Updates**
  - Add new item sprites for AI Legend gifts
  - Add special effect items for AI interactions
  - Add quest items for secret skill quests

## 2. AI Directory Structure

### AI/ Directory
```
AI/
├── legends/
│   ├── behaviors/           # AI Legend behavior patterns
│   ├── paths/              # Custom pathfinding data
│   ├── interactions/       # Interaction scripts
│   └── memory/            # Memory management configs
├── weather/
│   ├── patterns/          # Weather influence patterns
│   └── effects/          # Weather effect configs
└── system/
    ├── config.ini        # AI system configuration
    └── cache/            # AI response cache
```

### AI_sakray/ Directory
```
AI_sakray/
├── test/                 # Test configurations
├── debug/               # Debug settings
└── logs/               # AI system logs
```

## 3. Configuration Files

### DATA.INI Updates
```ini
[Data]
0=rdata.grf
1=data.grf
2=ai_legends.grf    # New GRF for AI content
```

### ai_client.ini (New File)
```ini
[AI]
enabled=true
memory_cache=true
response_timeout=5000
debug_level=1

[Legends]
show_indicators=true
effect_quality=high
sound_enabled=true

[Weather]
effects_enabled=true
transition_time=3000
```

## 4. Required DLL Updates

| File | Update Required | Purpose |
|------|----------------|---------|
| CDClient.dll | Yes | Add AI network protocol support |
| npkcrypt.dll | Yes | Add encryption for AI data |
| NPCIPHER.DLL | Yes | Update security for AI features |

## 5. Resource Requirements

### Memory
- Minimum: 4GB RAM
- Recommended: 8GB RAM
- Cache Size: 512MB

### Storage
- Additional Space: ~500MB
- GRF Updates: ~200MB
- AI Data: ~300MB

### Network
- Stable connection required
- Minimum 1Mbps upload/download
- Low latency recommended (<100ms)

## 6. Installation Steps

1. Back up existing client files
2. Update GRF files with new content
3. Install new DLL versions
4. Create AI directory structure
5. Configure AI settings
6. Verify file integrity

## 7. Validation Process

1. Verify GRF loading
```bash
grf_validate data.grf
grf_validate rdata.grf
```

2. Test AI resource loading
```bash
ai_test_resources
```

3. Validate DLL updates
```bash
verify_dll_versions
```

4. Check AI configuration
```bash
ai_config_check
```

## 8. Troubleshooting

### Common Issues
1. Missing AI sprites
   - Solution: Verify GRF installation
   - Check file permissions

2. AI response delays
   - Solution: Adjust cache settings
   - Check network connection

3. Memory issues
   - Solution: Clear AI cache
   - Adjust memory settings

## 9. Performance Optimization

### Client-side Caching
- Enable AI response caching
- Cache size: 256MB default
- Cache lifetime: 24 hours

### Graphics Settings
- Effect quality levels
- Animation smoothing
- Weather effect detail

## 10. Security Considerations

- Encrypted AI communications
- Secure memory management
- Protected AI resource files
- Client validation checks

## 11. Version Compatibility

Minimum supported versions:
- Client: 2022-04-06 or newer
- Ragexe: Build 1648707856 or newer
- AI System: 1.0.0 or newer

## 12. Backup and Recovery

1. Automatic backups:
   - GRF files
   - AI configurations
   - Memory cache

2. Recovery procedures:
   - Restore points
   - Configuration reset
   - Cache clearing