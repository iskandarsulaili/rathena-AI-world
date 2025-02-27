#!/usr/bin/env python3

import os
import sys
import shutil
import logging
from datetime import datetime
from pathlib import Path

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

class SimplifiedClientUpdater:
    def __init__(self, client_path):
        self.client_path = Path(client_path)
        self.system_path = self.client_path / "System"
        self.backup_dir = Path("./backup") / datetime.now().strftime("%Y%m%d_%H%M%S")
        self.dll_files = ["ai_client.dll", "ai_network.dll", "ai_resource.dll"]
        self.config_files = ["ai_client.ini", "ai_mapping.ini"]
        
        # Source paths (relative to script location)
        self.source_dll_dir = Path("./dlls")
        self.source_config_dir = Path("./configs")

    def _create_backup(self, file_path):
        """Create a backup of a file if it exists"""
        if not file_path.exists():
            return True
            
        try:
            backup_path = self.backup_dir / file_path.relative_to(self.client_path)
            backup_path.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(file_path, backup_path)
            logging.info(f"Created backup: {backup_path}")
            return True
        except Exception as e:
            logging.error(f"Backup failed: {e}")
            return False

    def _verify_client_directory(self):
        """Verify the client directory exists and has expected structure"""
        if not self.client_path.exists():
            logging.error(f"Client directory not found: {self.client_path}")
            return False
            
        # Check for key files that should exist in a valid client
        required_files = ["data.grf", "rdata.grf"]
        for file in required_files:
            if not (self.client_path / file).exists():
                logging.error(f"Required client file not found: {file}")
                logging.error("Please specify a valid Ragnarok Online client directory")
                return False
                
        return True

    def _create_system_directory(self):
        """Create System directory if it doesn't exist"""
        if not self.system_path.exists():
            try:
                self.system_path.mkdir(parents=True)
                logging.info(f"Created System directory: {self.system_path}")
            except Exception as e:
                logging.error(f"Failed to create System directory: {e}")
                return False
        return True

    def _install_dll_files(self):
        """Install DLL files to client directory"""
        for dll_file in self.dll_files:
            source_path = self.source_dll_dir / dll_file
            dest_path = self.client_path / dll_file
            
            if not source_path.exists():
                logging.error(f"Source DLL not found: {source_path}")
                return False
                
            # Backup existing file if present
            self._create_backup(dest_path)
            
            try:
                shutil.copy2(source_path, dest_path)
                logging.info(f"Installed: {dest_path}")
            except Exception as e:
                logging.error(f"Failed to install {dll_file}: {e}")
                return False
                
        return True

    def _install_config_files(self):
        """Install configuration files to System directory"""
        for config_file in self.config_files:
            source_path = self.source_config_dir / config_file
            dest_path = self.system_path / config_file
            
            if not source_path.exists():
                logging.error(f"Source config not found: {source_path}")
                return False
                
            # Backup existing file if present
            self._create_backup(dest_path)
            
            try:
                shutil.copy2(source_path, dest_path)
                logging.info(f"Installed: {dest_path}")
            except Exception as e:
                logging.error(f"Failed to install {config_file}: {e}")
                return False
                
        return True

    def _verify_installation(self):
        """Verify all files were installed correctly"""
        all_valid = True
        
        # Check DLL files
        for dll_file in self.dll_files:
            dll_path = self.client_path / dll_file
            if not dll_path.exists():
                logging.error(f"Verification failed: {dll_path} not found")
                all_valid = False
                
        # Check config files
        for config_file in self.config_files:
            config_path = self.system_path / config_file
            if not config_path.exists():
                logging.error(f"Verification failed: {config_path} not found")
                all_valid = False
                
        if all_valid:
            logging.info("Verification successful: All files installed correctly")
            
        return all_valid

    def update(self):
        """Perform the client update"""
        logging.info(f"Starting simplified client update for: {self.client_path}")
        
        # Step 1: Verify client directory
        if not self._verify_client_directory():
            return False
            
        # Step 2: Create System directory if needed
        if not self._create_system_directory():
            return False
            
        # Step 3: Install DLL files
        if not self._install_dll_files():
            return False
            
        # Step 4: Install config files
        if not self._install_config_files():
            return False
            
        # Step 5: Verify installation
        if not self._verify_installation():
            return False
            
        logging.info("Client update completed successfully!")
        return True

def main():
    if len(sys.argv) != 2:
        print("Usage: update_client_simplified.py <path_to_client_directory>")
        print("Example: update_client_simplified.py \"C:\\Program Files\\Ragnarok Online\\Client\"")
        sys.exit(1)

    client_path = sys.argv[1]
    updater = SimplifiedClientUpdater(client_path)
    success = updater.update()
    
    if success:
        print("\nUpdate completed successfully!")
        print("You can now launch the game to use the AI features.")
    else:
        print("\nUpdate failed. Please check the error messages above.")
        
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()