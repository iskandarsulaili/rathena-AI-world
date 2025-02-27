# Quick Start Guide: Simplified Client Updates for AI Features

This guide provides simplified instructions for updating your Ragnarok Online client to support the new AI features, focusing on adding new files rather than modifying existing ones.

## What You Need

- Original RO client files
- The three AI DLL files
- Two configuration files
- About 10MB of free disk space

## Installation Steps

### 1. Install DLL Files

Copy these DLL files to your main Client directory:

- `ai_client.dll`
- `ai_network.dll`
- `ai_resource.dll`

Example location: `C:\Program Files\Ragnarok Online\Client\`

### 2. Create System Directory (if needed)

If your client doesn't already have a System directory, create one:

```
Client/System/
```

### 3. Install Configuration Files

Copy these configuration files to the System directory:

- `ai_client.ini`
- `ai_mapping.ini`

Example location: `C:\Program Files\Ragnarok Online\Client\System\`

### 4. Verify Installation

Check that all files are in the correct locations:
- 3 DLL files in the Client directory
- 2 INI files in the Client/System directory

## Configuration Options

### AI Client Settings (ai_client.ini)

```ini
[AI]
enabled=true           # Set to false to disable AI features
memory_cache=true      # Improves performance
response_timeout=5000  # Milliseconds
debug_level=1          # 0-3, higher = more logging

[Legends]
show_indicators=true   # Visual indicators for AI characters
effect_quality=high    # Options: low, medium, high
sound_enabled=true     # AI character sounds

[Weather]
effects_enabled=true   # AI-influenced weather
transition_time=3000   # Weather transition speed
```

### Performance Tips

For better performance:
- Set `effect_quality=medium` or `effect_quality=low`
- Keep `memory_cache=true`
- Set `debug_level=0` for normal gameplay

## How It Works

This update uses existing game assets to implement AI features:

1. **AI Legends**: Uses existing character sprites mapped to new class IDs
2. **AI Skills**: Uses existing skill effects mapped to new skill IDs
3. **AI Maps**: Uses existing maps for AI Legend spawns

No GRF modifications are needed!

## Troubleshooting

### Common Issues

1. **Client crashes on startup**
   - Make sure all three DLL files are in the correct location
   - Check that your client version is compatible (2022-04-06 or newer)

2. **AI features not appearing**
   - Check that `enabled=true` in ai_client.ini
   - Verify that both INI files are in the System directory

3. **Performance problems**
   - Lower the `effect_quality` setting
   - Make sure `memory_cache` is enabled

### Getting Help

If you encounter issues:
1. Check the logs in `Client/System/ai_logs/`
2. Take screenshots of any error messages
3. Note your client version and operating system

## Compatibility

- Works with client version 2022-04-06 or newer
- Compatible with Windows 7/10/11
- Works with existing GRF files without modification

## Uninstallation

To remove AI features:
1. Delete the three DLL files from the Client directory
2. Delete the two INI files from the System directory

Or simply set `enabled=false` in ai_client.ini to temporarily disable.

## What's Included

### New DLL Files
- **ai_client.dll**: Core AI functionality
- **ai_network.dll**: Network communication
- **ai_resource.dll**: Resource management

### New Configuration Files
- **ai_client.ini**: General AI settings
- **ai_mapping.ini**: Maps AI content to existing assets

## Security Notes

- All DLL files are digitally signed
- No modifications to your existing game files
- No online authentication required
- No personal data collection

## Next Steps

After installation:
1. Launch the game
2. Look for AI Legend characters in major cities
3. Interact with them to experience new content
4. Explore the world to discover AI-influenced events