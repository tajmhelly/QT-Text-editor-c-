# Professional C++ Text Editor

A feature-rich, industry-class text editor built with Qt 6 and C++17.

## Features

### Core Functionality

- **Multi-tab editing** - Open and manage multiple documents
- **File operations** - Create, open, save, save-as with dialog support
- **Recent files** - Quick access to recently opened files
- **Drag & drop** - Drag files directly into the editor

### Text Editing

- **Line numbers** - Toggleable line number display
- **Word wrap** - Toggle word wrap mode
- **Syntax highlighting** - Support for multiple languages:
  - C/C++
  - Python
  - JavaScript/TypeScript
  - JSON
  - XML/HTML
  - CSS
  - SQL
- **Line operations**:
  - Delete line
  - Duplicate line
  - Move line up/down
  - Join lines

### Advanced Features

- **Undo/Redo** - Comprehensive undo/redo system with command merging
- **Search & Replace** - Advanced search with:
  - Case sensitivity
  - Whole word matching
  - Regular expressions
  - Replace all functionality
- **Font customization**:
  - Increase/decrease font size with Ctrl+/Ctrl-
  - Ctrl+wheel for quick sizing
- **Themes** - Light and dark theme support

### Professional Architecture

- **Modular design** - Separate concerns with dedicated managers
- **Memory efficient** - Smart pointers, proper resource management
- **Settings persistence** - Window state, recent files, preferences
- **Backup system** - Automatic backup creation
- **Session management** - Save and restore open files

## Project Structure

```
TextEditor/
├── CMakeLists.txt           # Build configuration
├── include/                 # Header files
│   ├── mainwindow.h
│   ├── editor.h
│   ├── documentmanager.h
│   ├── undoredostack.h
│   ├── searchreplace.h
│   └── syntaxhighlighter.h
├── src/                     # Implementation files
│   ├── main.cpp
│   ├── mainwindow.cpp
│   ├── editor.cpp
│   ├── documentmanager.cpp
│   ├── undoredostack.cpp
│   ├── searchreplace.cpp
│   └── syntaxhighlighter.cpp
├── ui/                      # UI files
│   └── mainwindow.ui
├── resources/               # Resource files
│   └── resources.qrc
└── build/                   # Build directory

```

## Building

### Requirements

- Qt 6.x
- CMake 3.16+
- C++17 compatible compiler

### Build Steps

```bash
cd TextEditor
mkdir build && cd build
cmake ..
make
./bin/TextEditor
```

## Architecture Overview

### MainWindow

- Central hub for all UI operations
- Manages menu bars, toolbars, status bar
- Handles document tab management
- Coordinates between components

### Editor

- Custom QPlainTextEdit with enhanced features
- Line number display with caching
- Syntax highlighting integration
- Advanced text operations
- Mouse and keyboard event handling

### DocumentManager

- File I/O operations
- Recent files tracking
- Backup management
- Session persistence
- File encoding detection

### UndoRedoStack

- Stack-based command system
- Command merging for efficiency
- Configurable stack size
- History inspection

### SearchReplace

- Multi-pattern search engine
- Regex support
- Case-sensitive searching
- Whole word matching
- Replace all operations
- Search result navigation

### SyntaxHighlighter

- Language detection from file extension
- Theme support (light/dark)
- Extensible rule system
- Multi-line comment handling
- Custom highlighting rules

## Keyboard Shortcuts

| Action        | Shortcut     |
| ------------- | ------------ |
| New           | Ctrl+N       |
| Open          | Ctrl+O       |
| Save          | Ctrl+S       |
| Save As       | Ctrl+Shift+S |
| Close         | Ctrl+W       |
| Exit          | Ctrl+Q       |
| Undo          | Ctrl+Z       |
| Redo          | Ctrl+Y       |
| Cut           | Ctrl+X       |
| Copy          | Ctrl+C       |
| Paste         | Ctrl+V       |
| Select All    | Ctrl+A       |
| Find          | Ctrl+F       |
| Replace       | Ctrl+H       |
| Find Next     | F3           |
| Find Previous | Shift+F3     |
| Increase Font | Ctrl++       |
| Decrease Font | Ctrl+-       |

## Configuration

Settings are stored in platform-specific locations:

- **Windows**: `%APPDATA%/TextEditor/TextEditor/`
- **Linux**: `~/.config/TextEditor/TextEditor/`
- **macOS**: `~/Library/Application Support/TextEditor/TextEditor/`

## Future Enhancements

- [ ] Plugin system
- [ ] Code folding
- [ ] Bracket matching
- [ ] Code completion
- [ ] Git integration
- [ ] Multi-cursor editing
- [ ] Split view
- [ ] Custom themes editor
- [ ] Terminal integration

## License

MIT License - See LICENSE file for details

## Author

Professional Development Team
