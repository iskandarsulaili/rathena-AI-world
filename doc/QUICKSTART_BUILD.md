# rAthena Quick Start Build Guide

This is a simplified guide for quickly building rAthena. For detailed instructions and troubleshooting, see `BUILD_PREREQUISITES.md` and `BUILD_INSTRUCTIONS.md`.

## Ubuntu/Debian Quick Start

1. Install prerequisites:
```bash
sudo apt update
sudo apt install git cmake make g++ libmysqlclient-dev libpcre3-dev zlib1g-dev
```

2. Clone and build:
```bash
# Clone repository
git clone https://github.com/your-org/rathena-AI-world.git
cd rathena-AI-world

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)
```

That's it! Your executables will be in the repository root directory.

## Windows Quick Start

1. Install required software:
   - Visual Studio Community (with "Desktop development with C++")
   - MySQL Community Server
   - CMake
   - Git

2. Clone and build:
```cmd
# Clone repository
git clone https://github.com/your-org/rathena-AI-world.git
cd rathena-AI-world

# Build
mkdir build && cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release
```

The executables will be created in the repository root directory.

## Verifying the Build

Check that these executables were created:
- login-server (login-server.exe on Windows)
- char-server (char-server.exe on Windows)
- map-server (map-server.exe on Windows)
- web-server (web-server.exe on Windows)

Test the login server:
```bash
# Linux
./login-server --run-once

# Windows
login-server.exe --run-once
```

## Next Steps

1. Copy sample configs:
```bash
cp conf/import-tmpl/* conf/import/
```

2. Configure database connection in:
- conf/import/inter_conf.txt
- conf/import/login_conf.txt

3. Import SQL files:
```sql
mysql -u root -p rathena < sql-files/main.sql
mysql -u root -p rathena < sql-files/logs.sql
```

For more detailed information:
- See `BUILD_PREREQUISITES.md` for complete prerequisites
- See `BUILD_INSTRUCTIONS.md` for detailed build instructions and options
- Check the project wiki for configuration and running instructions