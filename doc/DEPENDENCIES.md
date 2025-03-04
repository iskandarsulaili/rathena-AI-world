# rAthena Dependencies Guide

This document details all dependencies required for building and running rAthena.

## Build-time Dependencies

### Required Build Tools

| Dependency | Minimum Version | Ubuntu Package | Windows Source |
|------------|----------------|----------------|----------------|
| CMake | 3.5 | `cmake` | [cmake.org](https://cmake.org/download/) |
| C++ Compiler | GCC 7+ / MSVC 2019+ | `g++` | Visual Studio |
| Make | GNU Make 4.1+ | `make` | Included with VS |
| Git | 2.0+ | `git` | [git-scm.com](https://git-scm.com/download/win) |

### Required Libraries

| Library | Minimum Version | Ubuntu Package | Windows Source | Purpose |
|---------|----------------|----------------|----------------|----------|
| MySQL Client | 5.7 | `libmysqlclient-dev` | MySQL Installer | Database connectivity |
| PCRE | 8.0 | `libpcre3-dev` | [PCRE](https://pcre.org/) | Regular expressions |
| zlib | 1.2 | `zlib1g-dev` | [zlib](https://www.zlib.net/) | Compression |

### Optional Build Dependencies

| Dependency | Purpose | Ubuntu Package | Windows Source |
|------------|---------|----------------|----------------|
| ccache | Faster rebuilds | `ccache` | [ccache](https://ccache.dev/) |
| Ninja | Alternative build system | `ninja-build` | vcpkg |

## Runtime Dependencies

### Core Dependencies

| Dependency | Minimum Version | Ubuntu Package | Windows Requirement | Purpose |
|------------|----------------|----------------|---------------------|----------|
| MySQL Server | 5.7 | `mysql-server` | MySQL Installer | Database server |
| libmysqlclient | 5.7 | `libmysqlclient21` | Included with MySQL | MySQL client library |
| libpcre | 8.0 | `libpcre3` | Bundled | Regular expressions |
| zlib | 1.2 | `zlib1g` | Bundled | Compression |

### Optional Runtime Dependencies

| Dependency | Purpose | Ubuntu Package | Windows Source |
|------------|---------|----------------|----------------|
| MySQL GUI Tools | Database management | `mysql-workbench` | MySQL Installer |
| phpMyAdmin | Web-based DB admin | `phpmyadmin` | Manual install |

## Managing Dependencies

### Ubuntu/Debian

1. Install all build dependencies:
```bash
sudo apt update
sudo apt install cmake make g++ libmysqlclient-dev libpcre3-dev zlib1g-dev
```

2. Install runtime dependencies:
```bash
sudo apt install mysql-server libmysqlclient21 libpcre3 zlib1g
```

3. Optional development tools:
```bash
sudo apt install ccache ninja-build mysql-workbench
```

### Windows

1. Using Visual Studio Installer:
   - Install "Desktop development with C++"
   - Select "Windows 10/11 SDK"
   - Select "C++ CMake tools"

2. Using MySQL Installer:
   - Install MySQL Server
   - Install MySQL Workbench (optional)
   - Install Connector/C++
   - Install Visual C++ Redistributables

3. Manual installations:
   - CMake: Download and install from cmake.org
   - Git: Download and install from git-scm.com

## Dependency Configuration

### MySQL Configuration

1. Set up root password:
```bash
# Ubuntu
sudo mysql_secure_installation

# Windows
# Done during MySQL installation
```

2. Create database:
```sql
CREATE DATABASE rathena;
CREATE USER 'rathena'@'localhost' IDENTIFIED BY 'password';
GRANT ALL PRIVILEGES ON rathena.* TO 'rathena'@'localhost';
FLUSH PRIVILEGES;
```

### Environment Variables (Windows)

Add to system environment variables:
```cmd
MYSQL_BASE_DIR=C:\Program Files\MySQL\MySQL Server 8.0
CMAKE_PREFIX_PATH=%MYSQL_BASE_DIR%
```

## Verifying Dependencies

### Ubuntu
```bash
# Check MySQL
mysql --version
systemctl status mysql

# Check development libraries
ldconfig -p | grep -E 'libmysqlclient|libpcre|libz'

# Check build tools
cmake --version
g++ --version
```

### Windows
```cmd
# Check MySQL
mysql --version
sc query MySQL80

# Check Visual Studio
cl.exe
cmake --version
```

## Troubleshooting

### Common Issues

1. MySQL connection failed:
   - Check MySQL service is running
   - Verify credentials in configuration
   - Check firewall settings

2. Missing libraries:
   - Ubuntu: Install missing -dev packages
   - Windows: Check environment variables

3. Build errors:
   - Update CMake
   - Install/update Visual Studio components
   - Check compiler requirements