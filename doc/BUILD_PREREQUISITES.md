# Building rAthena - Prerequisites

## Common Requirements
- Git (for cloning the repository)
- CMake 3.5 or later
- C++ compiler with C++17 support
- MySQL 5.7 or later (including MariaDB)
- PCRE library
- zlib development files

## Ubuntu/Debian Prerequisites

Install required packages:
```bash
sudo apt update
sudo apt install git cmake make g++ libmysqlclient-dev libpcre3-dev zlib1g-dev
```

## Windows Prerequisites

1. Visual Studio
   - Download and install [Visual Studio Community](https://visualstudio.microsoft.com/downloads/)
   - Required components:
     * Desktop development with C++
     * Windows 10/11 SDK
     * C++ CMake tools for Windows

2. MySQL
   - Download and install [MySQL Community Server](https://dev.mysql.com/downloads/mysql/)
   - During installation:
     * Choose "Developer Default" or "Custom" install
     * Install MySQL Server and Connector/C++
     * Note the root password you set during installation

3. CMake
   - Download and install [CMake](https://cmake.org/download/)
   - During installation, choose to add CMake to system PATH

4. Git
   - Download and install [Git for Windows](https://git-scm.com/download/win)

## Verifying Prerequisites

### Ubuntu/Debian
```bash
# Check gcc version
gcc --version  # Should be 7.0 or higher

# Check cmake version
cmake --version  # Should be 3.5 or higher

# Check MySQL
mysql --version

# Verify PCRE and zlib are installed
ldconfig -p | grep -E 'libpcre|libz'
```

### Windows
1. Open Command Prompt and verify:
```cmd
cmake --version
git --version
```

2. Open Visual Studio and verify C++ development tools are installed
3. Verify MySQL is running:
```cmd
mysql --version