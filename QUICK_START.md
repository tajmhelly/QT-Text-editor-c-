# Professional C++ Text Editor - Quick Start

## ✅ Build Complete!

**Status**: Successfully compiled and working!
**Binary Location**: `/tmp/TextEditor/build/bin/TextEditor`
**Launcher Script**: `/tmp/TextEditor/run.sh`

## Running the Application

### Method 1: Using Launcher Script (Recommended)
```bash
/tmp/TextEditor/run.sh
```

### Method 2: Using Dynamic Loader Directly
```bash
/lib64/ld-linux-x86-64.so.2 \
  --library-path /lib/x86_64-linux-gnu:/usr/lib/x86_64-linux-gnu \
  /tmp/TextEditor/build/bin/TextEditor
```

### Method 3: Build from Source
```bash
cd /tmp/TextEditor
mkdir build && cd build
cmake ..
make -j$(nproc)
./bin/TextEditor
```

## Features

✅ **Multi-tab editing** - Switch between multiple files easily
✅ **Syntax highlighting** - C++, Python, JavaScript, JSON, XML, SQL, HTML, CSS
✅ **Line numbers** - Toggle on/off with View menu
✅ **Undo/Redo** - Smart command merging for efficient history
✅ **Search & Replace** - With regex support and case sensitivity options
✅ **Advanced editing**:
  - Duplicate line (Ctrl+D)
  - Delete line
  - Move line up/down
  - Join lines
✅ **Font control** - Ctrl++ to increase, Ctrl+- to decrease
✅ **File operations** - Create, Open, Save, Save As
✅ **Recent files** - Quick access menu
✅ **Word wrap** - Toggle from View menu
✅ **Settings persistence** - Remembers your preferences

## Keyboard Shortcuts

| Action | Shortcut |
|--------|----------|
| New File | Ctrl+N |
| Open File | Ctrl+O |
| Save File | Ctrl+S |
| Save As | Ctrl+Shift+S |
| Close File | Ctrl+W |
| Exit | Ctrl+Q |
| Undo | Ctrl+Z |
| Redo | Ctrl+Y |
| Cut | Ctrl+X |
| Copy | Ctrl+C |
| Paste | Ctrl+V |
| Select All | Ctrl+A |
| Find | Ctrl+F |
| Replace | Ctrl+H |
| Find Next | F3 |
| Find Previous | Shift+F3 |
| Increase Font | Ctrl++ |
| Decrease Font | Ctrl+- |

## Project Architecture

### Core Modules

**1. MainWindow** (`mainwindow.h/cpp`)
- Central UI hub with menu bar, toolbars, status bar
- Tab management for multiple documents
- Keyboard shortcut binding
- Settings integration

**2. Editor** (`editor.h/cpp`)
- Custom QPlainTextEdit extension
- Line number display with dynamic width
- Syntax highlighting integration
- Advanced text operations
- Mouse wheel font resizing (Ctrl+wheel)

**3. DocumentManager** (`documentmanager.h/cpp`)
- File I/O operations
- Recent files tracking (up to 10 files)
- Automatic backup creation
- Session management
- File encoding detection
- Settings persistence

**4. UndoRedoStack** (`undoredostack.h/cpp`)
- Stack-based undo/redo system
- Command merging for similar operations
- Configurable stack size (default 500)
- Merge timeout (default 500ms)
- Operation history inspection

**5. SearchReplace** (`searchreplace.h/cpp`)
- Multi-pattern search engine
- Regex support with validation
- Case sensitivity option
- Whole word matching
- Find next/previous navigation
- Replace single or all occurrences
- Search result highlighting

**6. SyntaxHighlighter** (`syntaxhighlighter.h/cpp`)
- Language detection from file extension
- Theme support (Light/Dark)
- Multi-language support:
  - C/C++
  - Python
  - JavaScript/TypeScript
  - JSON
  - XML
  - HTML
  - CSS
  - SQL

## Supported File Types

**Programming Languages**:
- `.cpp`, `.cc`, `.cxx`, `.h`, `.hpp` - C/C++
- `.c` - C
- `.py` - Python
- `.js`, `.ts` - JavaScript/TypeScript
- `.java` - Java
- `.cs` - C#
- `.go` - Go
- `.rs` - Rust
- `.php` - PHP

**Data Formats**:
- `.json` - JSON
- `.xml` - XML
- `.html`, `.htm` - HTML
- `.css` - CSS
- `.sql` - SQL

**Configuration**:
- `.yml`, `.yaml` - YAML
- `.toml` - TOML
- `.ini`, `.conf`, `.config` - Config files

**Other**:
- `.txt` - Plain text
- `.md` - Markdown
- `.sh`, `.bash` - Shell scripts

## Building Requirements

- **C++ Standard**: C++17
- **Qt Framework**: Qt 6.4+
- **Build System**: CMake 3.16+
- **Compiler**: GCC 11+ or Clang 12+

## Project Statistics

- **Lines of Code**: ~3000+
- **Header Files**: 6
- **Implementation Files**: 6
- **Total Classes**: 8
- **Signals/Slots**: 20+

## Configuration Files

Settings are stored at:
- **Linux**: `~/.config/TextEditor/TextEditor/`
- **Windows**: `%APPDATA%/TextEditor/TextEditor/`
- **macOS**: `~/Library/Application Support/TextEditor/TextEditor/`

## Future Enhancements

- [ ] Code folding
- [ ] Bracket matching/highlighting
- [ ] Code completion
- [ ] Multi-cursor editing
- [ ] Split view
- [ ] Custom theme editor
- [ ] Plugin system
- [ ] Git integration
- [ ] Terminal integration

## License

MIT License

## Contact & Support

For issues or improvements, please refer to the project documentation.

---

**Built with**: C++17 + Qt6 + CMake  
**Date**: February 2026
