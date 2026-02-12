# Build Instructions

## Quick Start

### Linux/macOS

```bash
cd TextEditor
mkdir build && cd build
cmake ..
make -j$(nproc)
./bin/TextEditor
```

### Windows (MSVC)

```cmd
cd TextEditor
mkdir build
cd build
cmake ..
cmake --build . --config Release
bin\TextEditor.exe
```

### Windows (MinGW)

```cmd
cd TextEditor
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
bin\TextEditor.exe
```

## Troubleshooting

### Qt6 Not Found

Ensure Qt6 is installed and in PATH:

```bash
# Linux
export Qt6_DIR=/path/to/qt/6.x/lib/cmake/Qt6

# macOS
export Qt6_DIR=/usr/local/opt/qt/lib/cmake/Qt6

# Windows
set Qt6_DIR=C:\Qt\6.x\lib\cmake\Qt6
```

### CMake Not Found

Install CMake:

```bash
# Ubuntu/Debian
sudo apt-get install cmake

# macOS
brew install cmake

# Windows
choco install cmake
```

## Development Setup

### IDE Integration

**Visual Studio Code:**

- Install C/C++ Extension
- Install CMake Tools Extension
- Configure Qt path in settings

**Qt Creator:**

1. Open TextEditor/CMakeLists.txt
2. Select Qt 6.x kit
3. Configure and build

**Visual Studio:**

- Use "Open Folder"
- CMakeSettings.json will be auto-generated
- Select configuration and build
