# Client Update Tools

This directory contains tools for updating Ragnarok Online clients to support the AI features in rAthena AI World.

## Simplified Update Approach

The simplified update approach focuses on adding new files rather than modifying existing GRF files. This makes the update process easier and less error-prone.

### Required Files

The update tools expect the following directory structure:

```
tools/
├── configs/                # Configuration files
│   ├── ai_client.ini      # AI client settings
│   └── ai_mapping.ini     # Mapping AI content to existing assets
├── dlls/                  # DLL files
│   ├── ai_client.dll      # Core AI functionality
│   ├── ai_network.dll     # Network communication
│   └── ai_resource.dll    # Resource management
├── update_client_simplified.py    # Python update script
├── update_client_simplified.bat   # Windows launcher
└── update_client_simplified.sh    # Linux/Mac launcher
```

### DLL Files

The DLL files provide the core functionality for the AI features:

- **ai_client.dll**: Handles AI protocol and client-side processing
- **ai_network.dll**: Manages network communication for AI features
- **ai_resource.dll**: Handles resource loading and caching for AI

### Configuration Files

The configuration files control how the AI features work:

- **ai_client.ini**: General AI settings
- **ai_mapping.ini**: Maps AI content to existing assets

## How to Use

### Windows Users

1. Make sure you have Python 3.7 or later installed
2. Double-click `update_client_simplified.bat`
3. Enter the path to your Ragnarok Online client directory when prompted
4. Wait for the update to complete

### Linux/Mac Users

1. Make sure you have Python 3.7 or later installed
2. Open a terminal in the tools directory
3. Make the script executable: `chmod +x update_client_simplified.sh`
4. Run the script: `./update_client_simplified.sh`
5. Enter the path to your Ragnarok Online client directory when prompted
6. Wait for the update to complete

## How It Works

The update process:

1. Verifies that all required files are present
2. Creates a backup of any existing files that will be modified
3. Copies the DLL files to the client directory
4. Creates the System directory if it doesn't exist
5. Copies the configuration files to the System directory
6. Verifies that all files were installed correctly

## Troubleshooting

If you encounter issues:

1. Check that Python is installed correctly
2. Make sure all required files are present in the correct directories
3. Check that the client directory path is correct
4. Look for error messages in the console output
5. Check the backup directory for any files that were backed up

## Advanced Configuration

You can customize the AI features by editing the configuration files:

### ai_client.ini

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

### ai_mapping.ini

This file maps AI content to existing game assets. You can customize these mappings to use different existing assets for the AI features.

## Uninstallation

To remove the AI features:

1. Delete the three DLL files from the client directory
2. Delete the two INI files from the System directory

Or simply set `enabled=false` in ai_client.ini to temporarily disable the features.