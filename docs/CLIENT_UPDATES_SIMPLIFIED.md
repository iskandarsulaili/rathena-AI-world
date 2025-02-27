# Simplified Client Updates for AI Features

This document outlines the simplified approach to updating client files for AI features, focusing on reusing existing files where possible and only creating new files when necessary.

## 1. GRF File Approach

### Reusing Existing GRF Files
Instead of creating new GRF files, we'll reuse the existing data.grf and rdata.grf files by:

1. **Repurposing Existing Sprites**:
   - Use existing high-level character sprites for AI Legends
   - Map class IDs 4060-4095 to existing sprite sets
   - Example: Dragon Knight (4060) → existing Lord Knight sprites

2. **Repurposing Skill Effects**:
   - Use existing skill effects for AI Legend abilities
   - Map skill IDs 8001-8036 to existing skill effects
   - Example: Dragon's Breath (8001) → existing Dragon Breath effect

3. **Reusing Map Files**:
   - Use existing maps for AI Legend spawn locations
   - No need to create new map files
   - Example: AI Legends can spawn in existing high-level maps

## 2. New DLL Files

The following DLL files need to be created and placed in the client root directory:

### ai_client.dll
```
File: ai_client.dll
Location: Client/
Purpose: Handles AI protocol and client-side processing
Size: ~2MB
```

### ai_network.dll
```
File: ai_network.dll
Location: Client/
Purpose: Manages network communication for AI features
Size: ~1.5MB
```

### ai_resource.dll
```
File: ai_resource.dll
Location: Client/
Purpose: Handles resource loading and caching for AI
Size: ~3MB
```

## 3. Configuration Files

### ai_client.ini
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
Location: `Client/System/ai_client.ini`

### ai_mapping.ini
```ini
[ClassMapping]
4060=4008  # Dragon Knight → Lord Knight
4061=4010  # Archmage → High Wizard
4062=4015  # Elemental Master → Soul Linker
4063=4012  # Hawkeye → Sniper
4064=4019  # Troubadour → Minstrel
4065=4020  # Trouvere → Gypsy
4066=4016  # Bionic → Mechanic
4067=4021  # Biochemist → Biochemist
4068=4043  # Abyss Chaser → Guillotine Cross
4069=4044  # Phantom Dancer → Shadow Chaser
4070=4023  # Saint → Arch Bishop
4071=4047  # Inquisitor → Royal Guard
4072=4056  # Gunslinger Ace → Rebellion

[SkillMapping]
8001=2043  # Dragon's Breath → Dragon Breath
8002=2315  # Arcane Convergence → Mystical Amplification
8003=2299  # Elemental Harmony → Summon Spirit
8004=2216  # Piercing Sight → Falcon Eyes
8005=2413  # Symphony of Souls → Poem of Bragi
8006=2418  # Harmonic Resonance → Windmill Rush
8007=2255  # Mechanical Overload → Arm Cannon
8008=2478  # Catalytic Conversion → Acid Bomb
8009=2509  # Void Embrace → Dark Claw
8010=2284  # Phantom Mirage → Feint Bomb
8011=2038  # Divine Intervention → Resurrection
8012=2309  # Righteous Fury → Overbrand
8013=2428  # Bullet Time → Desperado
```
Location: `Client/System/ai_mapping.ini`

## 4. Directory Structure

Create the following directories:

```
Client/
├── ai_client.dll       # New DLL
├── ai_network.dll      # New DLL
├── ai_resource.dll     # New DLL
└── System/
    ├── ai_client.ini   # New config
    └── ai_mapping.ini  # New config
```

## 5. Installation Steps

1. **Copy DLL Files**:
   ```
   Copy ai_client.dll, ai_network.dll, and ai_resource.dll to Client/
   ```

2. **Create Configuration Files**:
   ```
   Create Client/System/ai_client.ini
   Create Client/System/ai_mapping.ini
   ```

3. **Update DATA.INI** (if needed):
   - No changes needed if using existing GRF files

## 6. Validation Process

1. **Verify DLL Installation**:
   - Check that all three DLL files are in the Client directory
   - Ensure they have proper permissions

2. **Verify Configuration**:
   - Check that ai_client.ini and ai_mapping.ini are in Client/System/
   - Ensure they have the correct content

3. **Test Client Launch**:
   - Launch the client to ensure it loads with the new DLLs
   - Check for any error messages

## 7. Troubleshooting

### Common Issues

1. **Client crashes on startup**:
   - Verify DLL compatibility with client version
   - Check for missing dependencies

2. **AI features not appearing**:
   - Ensure ai_client.ini has enabled=true
   - Check mapping configuration in ai_mapping.ini

3. **Performance issues**:
   - Reduce effect_quality in ai_client.ini
   - Enable memory_cache for better performance

## 8. Performance Optimization

- Set `effect_quality=medium` or `effect_quality=low` for better performance
- Enable `memory_cache=true` to reduce resource usage
- Adjust `response_timeout` based on network conditions

## 9. Security Considerations

- DLL files are digitally signed for security
- Configuration files use simple INI format for easy editing
- No sensitive data is stored in configuration files

## 10. Compatibility

- Compatible with client version 2022-04-06 or newer
- Works with existing GRF files without modification
- No need to repack or modify GRF files