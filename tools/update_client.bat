@echo off
setlocal enabledelayedexpansion

:: Set colors for better visibility
set "RED=31"
set "GREEN=32"
set "YELLOW=33"
set "BLUE=34"

echo [%BLUE%mChecking Python installation...
python --version >nul 2>&1
if errorlevel 1 (
    echo [%RED%mError: Python is not installed or not in PATH
    echo Please install Python 3.7 or later from https://www.python.org/
    echo [0m
    pause
    exit /b 1
)

echo [%GREEN%mPython found[0m

:: Check required Python packages
echo [%BLUE%mChecking required packages...[0m
python -c "import json" 2>nul
if errorlevel 1 (
    echo [%YELLOW%mInstalling required packages...[0m
    pip install json
)

:: Check if config file exists
if not exist "client_update_config.json" (
    echo [%RED%mError: client_update_config.json not found
    echo Please ensure you are running this script from the tools directory[0m
    pause
    exit /b 1
)

:: Create backup directory
if not exist "backup" (
    echo [%BLUE%mCreating backup directory...[0m
    mkdir backup
)

:: Check for required files
echo [%BLUE%mChecking required files...[0m
set MISSING=0
for %%F in (data.grf rdata.grf CDClient.dll) do (
    if not exist "..\Client\%%F" (
        echo [%RED%mMissing required file: %%F[0m
        set /a MISSING+=1
    )
)

if !MISSING! gtr 0 (
    echo [%RED%mError: Required files are missing. Please check the Client directory[0m
    pause
    exit /b 1
)

:: Run the update script
echo [%BLUE%mStarting client update process...[0m
python update_client.py client_update_config.json
if errorlevel 1 (
    echo [%RED%mError: Update process failed
    echo Please check the logs for more information[0m
    pause
    exit /b 1
)

echo [%GREEN%mUpdate completed successfully![0m
echo.
echo [%BLUE%mReminder: Please verify the game client works correctly after update
echo If you experience any issues, you can find backups in the backup directory[0m

pause
exit /b 0