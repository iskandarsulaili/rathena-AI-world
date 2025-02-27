# AI DLL Files

This directory should contain the following DLL files:

1. **ai_client.dll** - Core AI functionality
2. **ai_network.dll** - Network communication
3. **ai_resource.dll** - Resource management

## DLL Specifications

### ai_client.dll

- **Size:** ~2MB
- **Version:** 1.0.0
- **Purpose:** Handles AI protocol and client-side processing
- **Dependencies:** ai_network.dll, ai_resource.dll

This DLL provides the core functionality for the AI features, including:
- AI Legend character handling
- AI skill processing
- AI event management
- Configuration loading and parsing

### ai_network.dll

- **Size:** ~1.5MB
- **Version:** 1.0.0
- **Purpose:** Manages network communication for AI features
- **Dependencies:** None

This DLL handles all network communication for the AI features, including:
- Server-client protocol
- Data encryption/decryption
- Packet handling
- Connection management

### ai_resource.dll

- **Size:** ~3MB
- **Version:** 1.0.0
- **Purpose:** Handles resource loading and caching for AI
- **Dependencies:** None

This DLL manages resources for the AI features, including:
- Asset mapping
- Memory caching
- Resource loading
- Performance optimization

## Installation

These DLL files should be placed in the client's root directory:

```
Client/
├── ai_client.dll
├── ai_network.dll
├── ai_resource.dll
└── ...
```

## Development Notes

These DLLs are compiled for Windows and are compatible with the Ragnarok Online client version 2022-04-06 or newer. They are digitally signed for security.

The DLLs use a hook-based approach to integrate with the client without modifying any existing files. They intercept specific function calls to add the AI functionality.

## Troubleshooting

If the client crashes after installing these DLLs, check:
1. Client version compatibility
2. DLL version compatibility
3. Missing dependencies
4. File permissions

## Security

These DLLs:
- Do not collect any personal data
- Do not modify any existing game files
- Do not require online authentication
- Are digitally signed for security

## Source Code

The source code for these DLLs is available in the `src/client` directory of the rAthena AI World repository.