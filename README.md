# Wee Text Editor

A lightweight, terminal-based text editor built in C using the ncurses library, featuring efficient text manipulation through gap buffer data structures.

## Demo

https://github.com/user-attachments/assets/3f591a5e-cdc7-4810-af3d-b6d83b4dbcb3

## Overview

Wee is a minimal yet functional command-line text editor that demonstrates fundamental concepts in systems programming, data structures, and terminal I/O. The editor uses gap buffers for efficient character insertion and deletion, making it suitable for real-time text editing.

## Features

- **File Operations**: Open existing files or create new documents
- **Real-time Editing**: Insert, delete, and navigate through text seamlessly
- **Auto-save**: Changes automatically saved on exit (Ctrl+Q)
- **Multi-line Support**: Full support for line breaks and multi-line documents
- **Cursor Navigation**: Arrow key support for moving through text
- **Viewport Management**: Automatic scrolling for documents larger than terminal window
- **Line Manipulation**: Join lines on backspace, split lines on enter

## Key Technical Implementation

### Gap Buffer Architecture

The editor uses **gap buffers** as the core data structure for efficient text manipulation. Gap buffers maintain a "gap" (empty space) at the cursor position, enabling O(1) character insertion and deletion at the cursor.

**Structure:**
```c
typedef struct {
    char* data;           // Character array with gap
    int size;             // Total buffer size
    int insert_position;  // Start of gap
    int second_position;  // End of gap
} GapBuffer;
```

**Example Visualization:**
```
Before insertion: "Hello[    ]World"
After 'x' insert: "Hellox[   ]World"
```

**Key Operations:**
- `gap_insert_char()` - O(1) insertion at cursor
- `gap_remove_char()` - O(1) deletion at cursor
- `gap_set_insert_position()` - O(k) where k is distance moved (shifts gap)
- `gap_break()` - Split buffer at cursor for line breaks

### Document Structure

Documents are represented as **doubly-linked lists** of lines, with each line containing its own gap buffer:

```c
typedef struct line {
    GapBuffer* gbuf;
    struct line* next;
    struct line* previous;
} Line;

typedef struct {
    Line* head;
    Line* tail;
    int num_lines;
} Document;
```

This architecture allows:
- Efficient line insertion/deletion
- Fast navigation between lines
- Memory efficiency (only allocate what's needed)

### Window System

The `Window` struct provides a viewport into the document, handling scrolling and display:

```c
typedef struct {
    Document* document;
    int first_row;      // Top visible line
    int first_col;      // Left visible column
    int current;        // Current cursor line
    int width;          // Terminal width
    int height;         // Terminal height
} Window;
```

## Usage

### Building

```bash
# Build the editor
make

# Run with a file
./wee filename.txt

# Run with new file
./wee
```

### Keyboard Controls

| Key | Action |
|-----|--------|
| Arrow Keys | Navigate cursor |
| Backspace | Delete character (or join lines at line start) |
| Enter | Insert line break |
| Ctrl+Q | Save and exit |
| Any character | Insert at cursor position |

### Creating a New File

```bash
./wee
# Defaults to "unnamed.txt"
# Edit and press Ctrl+Q to save
```

### Editing Existing File

```bash
./wee myfile.txt
# Make changes
# Press Ctrl+Q to save and exit
```

## Project Structure

```
├── main.c              # Main editor loop and input handling
├── gap_buffer.c/h      # Gap buffer implementation
├── document.c/h        # Document (linked list of lines)
├── window.c/h          # Viewport management
├── terminal.c/h        # ncurses terminal abstraction
├── status.c/h          # Status bar display
├── log.c/h             # Debugging utilities
├── test/               # Google Test suite
│   └── test_wee.cpp    # Unit tests
└── Makefile            # Build configuration
```

## Technical Highlights

### Efficient Text Editing

**Gap Buffer Benefits:**
- O(1) insertion/deletion at cursor
- Memory efficient (single allocation per line)
- Reduces memory fragmentation compared to dynamic arrays

**Trade-offs:**
- Moving cursor requires shifting gap: O(k) where k = distance
- Optimal for sequential editing (typing), less optimal for random access

### Line Operations

**Smart Line Joining (Backspace at line start):**
```c
// Combines current line with previous line
char* toInsert = gap_to_string(currentline->gbuf);
gap_insert_string(currentline->previous->gbuf, strlen(toInsert), toInsert);
document_remove(document, currentline);
```

**Line Splitting (Enter key):**
```c
// Breaks current line at cursor position
newL->gbuf = gap_break(currentline->gbuf);
document_insert_after(document, currentline, newL);
```

### Memory Management

Proper cleanup on all allocation paths:
- `gap_free()` - Deallocates gap buffer data
- `document_free()` - Iterates through lines, freeing each
- `window_free()` - Releases window and associated document

## Testing

The project includes a Google Test suite for unit testing core components:

```bash
# Build and run all tests
make test

# Test specific components
make test-gap         # Gap buffer tests
make test-document    # Document structure tests
make test-document-io # File I/O tests
```

## Learning Outcomes

This project demonstrates:
- **Data Structures**: Gap buffers, doubly-linked lists, dynamic memory
- **Systems Programming**: Terminal I/O, ncurses, file operations
- **C Programming**: Pointers, manual memory management, struct composition
- **Software Design**: Separation of concerns (buffer, document, window, terminal)
- **Testing**: Unit testing in C/C++ with Google Test
- **Build Systems**: Makefile with multiple targets and dependencies

## Potential Future Enhancements

- Add syntax highlighting with language detection
- Implement undo/redo using command pattern
- Add search and replace with regex support
- Support for multiple buffers/tabs
- Line numbers display
- Configuration file for keybindings
- Mouse support via ncurses mouse events

## Dependencies

- **gcc**: C compiler
- **ncurses**: Terminal control library
- **Google Test** (optional): For running unit tests

Install on Ubuntu/Debian:
```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
sudo apt-get install libgtest-dev  # Optional, for tests
```

Install on macOS:
```bash
brew install ncurses
brew install googletest  # Optional, for tests
```