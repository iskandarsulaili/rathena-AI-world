# Building rAthena - Compilation Instructions

Make sure you have installed all prerequisites listed in `BUILD_PREREQUISITES.md` before proceeding.

## Building on Ubuntu/Debian

1. Clone the repository:
```bash
git clone https://github.com/your-org/rathena-AI-world.git
cd rathena-AI-world
```

2. Create and enter build directory:
```bash
mkdir build
cd build
```

3. Configure with CMake:
```bash
cmake -G "Unix Makefiles" ..
```

4. Build the project:
```bash
make -j$(nproc)  # Uses all available CPU cores
```

The compiled executables will be created in the repository root:
- login-server
- char-server
- map-server
- web-server
- mapcache
- csv2yaml
- yaml2sql
- yamlupgrade

## Building on Windows

1. Clone the repository:
```cmd
git clone https://github.com/your-org/rathena-AI-world.git
cd rathena-AI-world
```

2. Create build directory:
```cmd
mkdir build
cd build
```

3. Configure with CMake:
```cmd
cmake -G "Visual Studio 17 2022" -A x64 ..
```
Note: Replace "Visual Studio 17 2022" with your installed VS version if different

4. Build using either:

   a. Visual Studio GUI:
   - Open the generated `.sln` file in Visual Studio
   - Select "Release" configuration
   - Build -> Build Solution (F7)

   b. Command line:
   ```cmd
   cmake --build . --config Release
   ```

The compiled executables will be created in the repository root:
- login-server.exe
- char-server.exe
- map-server.exe
- web-server.exe
- mapcache.exe
- csv2yaml.exe
- yaml2sql.exe
- yamlupgrade.exe

## Common Build Options

You can customize the build by passing options to CMake:

```bash
# Example: Enable debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Example: Specify custom MySQL location
cmake -DMYSQL_BASE_DIR=/path/to/mysql ..
```

Common options:
- `CMAKE_BUILD_TYPE`: Set to Debug/Release/RelWithDebInfo
- `MYSQL_BASE_DIR`: Custom MySQL installation directory
- `PCRE_BASE_DIR`: Custom PCRE installation directory
- `ZLIB_ROOT`: Custom zlib installation directory

## Troubleshooting

### Common Issues on Ubuntu/Debian

1. Missing dependencies:
```bash
sudo apt install build-essential
```

2. MySQL development files not found:
```bash
sudo apt install libmysqlclient-dev
```

### Common Issues on Windows

1. MYSQL_BASE_DIR not found:
- Set the environment variable:
```cmd
set MYSQL_BASE_DIR=C:\Program Files\MySQL\MySQL Server 8.0
```
- Or specify in CMake:
```cmd
cmake -DMYSQL_BASE_DIR="C:\Program Files\MySQL\MySQL Server 8.0" ..
```

2. Visual Studio missing components:
- Open Visual Studio Installer
- Modify your installation
- Add "Desktop development with C++"

## Post-Build Steps

1. Configure MySQL:
- Create a database for rAthena
- Import SQL files from `sql-files/` directory

2. Copy and configure server settings:
- Copy sample configuration files from `conf/` directory
- Modify settings as needed (especially database connection)

3. Test the build:
```bash
./login-server --run-once  # Should start and connect to database