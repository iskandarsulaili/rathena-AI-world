# Quick Start Guide: Updating Client for AI Features

This guide will help you update your Ragnarok Online client to support the new AI features.

## Prerequisites

- Python 3.7 or later
- Original RO client files
- At least 1GB free disk space for backups
- Internet connection for downloading updates

## Update Process

### Windows Users

1. Navigate to the `tools` directory
2. Double-click `update_client.bat`
3. Follow the on-screen instructions

### Linux/Mac Users

1. Open terminal and navigate to the `tools` directory
2. Make the update script executable:
   ```bash
   chmod +x update_client.sh
   ```
3. Run the update script:
   ```bash
   ./update_client.sh
   ```

## What Gets Updated

- GRF Files (data.grf, rdata.grf)
  - New AI Legend character sprites
  - New skill effects
  - Updated maps
  - New sound effects

- DLL Files
  - CDClient.dll
  - npkcrypt.dll
  - NPCIPHER.DLL

- Configuration Files
  - DATA.INI
  - ai_client.ini (new)

## Backup and Recovery

- All original files are backed up before updating
- Backups are stored in the `backup` directory
- Each backup is timestamped

To restore from backup:
1. Navigate to the `backup` directory
2. Find the desired backup timestamp
3. Copy files back to their original locations

## Verifying the Update

After updating, verify that:
1. The client launches successfully
2. You can see AI Legend characters
3. AI-related features work properly

## Troubleshooting

### Common Issues

1. **Update fails to start**
   - Make sure Python is installed
   - Run as administrator on Windows
   - Check file permissions on Linux/Mac

2. **Missing files error**
   - Verify original client files are present
   - Check file paths in configuration

3. **Client crashes after update**
   - Restore from backup
   - Try running update again
   - Check for antivirus interference

### Getting Help

If you encounter issues:
1. Check the logs in `tools/logs`
2. Take screenshots of any error messages
3. Contact support with:
   - Error logs
   - Screenshots
   - System information

## System Requirements

### Minimum
- OS: Windows 7/Ubuntu 18.04/macOS 10.14
- RAM: 4GB
- Storage: 2GB free space
- CPU: Dual-core processor

### Recommended
- OS: Windows 10/Ubuntu 20.04/macOS 11
- RAM: 8GB
- Storage: 5GB free space
- CPU: Quad-core processor

## Performance Tips

1. Clear the AI cache regularly:
   - Delete contents of `AI/system/cache`
   - Keep `AI/system/config.ini`

2. Optimize settings in ai_client.ini:
   ```ini
   [AI]
   memory_cache=true
   response_timeout=5000
   
   [Legends]
   effect_quality=medium  # Set to low for better performance
   ```

3. Regular maintenance:
   - Clear old backups
   - Update AI configurations
   - Monitor resource usage

## Feature Verification

After updating, test these features:
1. AI Legend character interactions
2. Special skill effects
3. Weather system changes
4. Sound effects
5. Map updates

## Safety Notes

1. Always keep backups
2. Don't modify AI configuration files manually
3. Use official update tools only
4. Report bugs and issues promptly

## Additional Resources

- [Full Documentation](./CLIENT_UPDATES.md)
- [AI Features Guide](./AI_ADDITIONAL_FEATURES.md)
- [Performance Optimization](./AI_AGENTS_OPTIMIZATION.md)
- [Security Guidelines](./SECURITY_REVIEW.md)