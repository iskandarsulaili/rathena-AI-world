@echo off
setlocal enabledelayedexpansion

:: Set colors for better visibility
set "RED=31"
set "GREEN=32"
set "YELLOW=33"
set "BLUE=34"

echo [%BLUE%mSimplified AI Client Updater[0m
echo ==============================
echo.

:: Check Python installation
echo [%BLUE%mChecking Python installation...[0m
python --version >nul 2>&1
if errorlevel 1 (
    echo [%RED%mError: Python is not installed or not in PATH
    echo Please install Python 3.7 or later from https://www.python.org/
    echo [0m
    pause
    exit /b 1
)

echo [%GREEN%mPython found[0m

:: Check if required directories exist
if not exist "dlls" (
    echo [%RED%mError: dlls directory not found
    echo Please ensure the dlls directory exists and contains the required DLL files[0m
    pause
    exit /b 1
)

if not exist "configs" (
    echo [%RED%mError: configs directory not found
    echo Please ensure the configs directory exists and contains the required configuration files[0m
    pause
    exit /b 1
)

:: Check for required DLL files
set MISSING=0
for %%F in (dlls\ai_client.dll dlls\ai_network.dll dlls\ai_resource.dll) do (
    if not exist "%%F" (
        echo [%RED%mMissing required file: %%F[0m
        set /a MISSING+=1
    )
)

:: Check for required config files
for %%F in (configs\ai_client.ini configs\ai_mapping.ini) do (
    if not exist "%%F" (
        echo [%RED%mMissing required file: %%F[0m
        set /a MISSING+=1
    )
)

if !MISSING! gtr 0 (
    echo [%RED%mError: Required files are missing. Please check the dlls and configs directories[0m
    pause
    exit /b 1
)

:: Ask for client directory
set "CLIENT_DIR="
echo.
echo [%BLUE%mPlease enter the path to your Ragnarok Online client directory:[0m
echo Example: C:\Program Files\Ragnarok Online\Client
echo.
set /p CLIENT_DIR="Client directory: "

:: Validate client directory
if not exist "%CLIENT_DIR%" (
    echo [%RED%mError: The specified directory does not exist[0m
    pause
    exit /b 1
)

if not exist "%CLIENT_DIR%\data.grf" (
    echo [%RED%mError: This does not appear to be a valid Ragnarok Online client directory
    echo (data.grf not found)[0m
    pause
    exit /b 1
)

:: Run the update script
echo.
echo [%BLUE%mStarting client update process...[0m
python update_client_simplified.py "%CLIENT_DIR%"
if errorlevel 1 (
    echo [%RED%mError: Update process failed
    echo Please check the logs for more information[0m
    pause
    exit /b 1
)

echo.
echo [%GREEN%mUpdate completed successfully![0m
echo.
echo [%BLUE%mYou can now launch the game to use the AI features.
echo If you experience any issues, you can find backups in the backup directory[0m

pause
exit /b 0