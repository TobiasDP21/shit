# Unity Reflection Viewer

C++ application with ImGui UI for viewing Unity Assembly-CSharp reflection data.

## Features

- Beautiful ImGui-based user interface
- Real-time connection to Unity via Named Pipes
- Type browser with search and filtering
- Detailed member views (Fields, Methods, Properties)
- Color-coded type categories
- Cross-platform support

## Prerequisites

### All Platforms
- CMake 3.15 or later
- C++17 compatible compiler
- Git

### Windows
- Visual Studio 2019 or later (with C++ workload)
- vcpkg (for GLFW3)

### Linux
```bash
sudo apt-get update
sudo apt-get install build-essential cmake
sudo apt-get install libglfw3-dev libgl1-mesa-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

### macOS
```bash
brew install cmake glfw
```

## ImGui Setup

Download Dear ImGui and place it in the `external/imgui` directory:

```bash
cd UnityReflectionViewer/external
git clone https://github.com/ocornut/imgui.git
```

Or download manually from: https://github.com/ocornut/imgui

The directory structure should look like:
```
external/
└── imgui/
    ├── imgui.cpp
    ├── imgui.h
    ├── imgui_demo.cpp
    ├── imgui_draw.cpp
    ├── imgui_tables.cpp
    ├── imgui_widgets.cpp
    ├── backends/
    │   ├── imgui_impl_glfw.cpp
    │   ├── imgui_impl_glfw.h
    │   ├── imgui_impl_opengl3.cpp
    │   └── imgui_impl_opengl3.h
    └── ...
```

## Building

### Windows (Visual Studio)

```bash
cd UnityReflectionViewer
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

Run:
```bash
.\Release\UnityReflectionViewer.exe
```

### Linux

```bash
cd UnityReflectionViewer
mkdir build && cd build
cmake ..
make -j$(nproc)
```

Run:
```bash
./UnityReflectionViewer
```

### macOS

```bash
cd UnityReflectionViewer
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

Run:
```bash
./UnityReflectionViewer
```

## Usage

1. **Start the Viewer**:
   ```bash
   ./UnityReflectionViewer
   ```
   The application will start and wait for Unity to connect.

2. **Start Unity**:
   - Open your Unity project with the UnityReflectionLib installed
   - Enter Play mode
   - The viewer will automatically connect and display the reflection data

3. **Browse the Data**:
   - **Type List (Left Panel)**:
     - Browse all types from Assembly-CSharp
     - Use the search box to filter types
     - Filter by type category (Classes, Structs, Enums, Interfaces)

   - **Type Details (Right Panel)**:
     - View selected type information
     - Switch between Fields, Methods, and Properties tabs
     - See full member signatures and metadata

## UI Guide

### Type List

Color-coded icons indicate type categories:
- **[C]** Blue - Class
- **[S]** Yellow - Struct
- **[E]** Purple - Enum
- **[I]** Green - Interface

### Filters

- **Search Box**: Type to filter by name or namespace
- **Category Checkboxes**: Show only selected type categories
- **View Menu**:
  - "Show Public Only" - Hide private members
  - "Show Inherited Members" - (Future feature)

### Type Details

**Header Section**:
- Type name and namespace
- Base type information
- Type category

**Tabs**:
1. **Fields Tab**:
   - Field name and type
   - Access modifier (Public/Private)
   - Static/Instance
   - ReadOnly flag

2. **Methods Tab**:
   - Method name and full signature
   - Return type
   - Parameters with types
   - Access modifier
   - Static/Instance

3. **Properties Tab**:
   - Property name and type
   - Get/Set accessors

## Architecture

### Components

1. **main.cpp**: Application entry point, GLFW/OpenGL setup
2. **ipc_client**: Named pipe client for IPC with Unity
3. **reflection_data**: Data models and JSON parser
4. **main_window**: ImGui UI implementation

### Communication Flow

```
Unity (C#)                    Viewer (C++)
    |                              |
    | 1. Start IPC Server          |
    |                              |
    |      2. Connect to Pipe      |
    |<-----------------------------|
    |                              |
    | 3. Send Reflection Data      |
    |----------------------------->|
    |                              |
    |      4. Parse & Display      |
    |                              |
```

### Data Format

Data is transmitted as JSON over Named Pipes:
- Length prefix (4 bytes, int32)
- JSON payload (UTF-8 encoded)

## Configuration

### Named Pipe Settings

**Windows**: `\\.\pipe\UnityReflectionPipe`
**Linux/Mac**: `/tmp/UnityReflectionPipe`

To change the pipe name, edit:
- C#: `IPCServer.cs` - `PipeName` constant
- C++: `ipc_client.h` - `PIPE_NAME` constant

### UI Customization

Edit `main_window.cpp` to customize:
- Window size and layout
- Color scheme
- Table columns
- Filter options

## Troubleshooting

### Build Errors

**"GLFW not found"**:
- Windows: Install via vcpkg: `vcpkg install glfw3`
- Linux: `sudo apt-get install libglfw3-dev`
- macOS: `brew install glfw`

**"imgui.h not found"**:
- Ensure ImGui is in `external/imgui/`
- Check that `backends/` subdirectory exists

### Runtime Errors

**"Failed to connect to pipe"**:
- Ensure Unity is running in Play mode
- Check that UnityReflectionManager is active
- Verify no firewall is blocking local IPC

**"Failed to parse assembly data"**:
- Check Unity console for serialization errors
- Verify JSON is valid (check C# IPCServer output)

### Connection Issues

**Viewer doesn't connect**:
1. Check Unity console for "IPC Server started" message
2. Verify the pipe names match in both applications
3. Try running both applications as administrator (Windows)
4. Check file permissions on `/tmp/UnityReflectionPipe` (Linux/Mac)

**Connection drops immediately**:
- This is normal behavior - Unity sends data once per connection
- The viewer will auto-reconnect when Unity restarts

## Performance

- **Startup Time**: ~1 second
- **Connection Time**: < 100ms
- **Data Transfer**: Depends on assembly size (typically < 1 second)
- **UI Rendering**: 60 FPS with vsync

Large assemblies (1000+ types) may take 2-3 seconds to parse and display.

## Development

### Adding New Features

1. **New Data Fields**:
   - Update `ReflectionData.cs` in Unity library
   - Update `reflection_data.h/cpp` in viewer
   - Update JSON serialization in both

2. **UI Enhancements**:
   - Modify `main_window.cpp`
   - Use ImGui documentation: https://github.com/ocornut/imgui

3. **IPC Improvements**:
   - Edit `ipc_client.cpp/h`
   - Consider using protobuf for binary serialization

### Code Structure

```cpp
// Main entry point
main.cpp
  └─> Creates GLFW window
  └─> Initializes ImGui
  └─> Creates IPCClient and MainWindow
  └─> Runs main render loop

// IPC handling
ipc_client.cpp
  └─> Connects to named pipe
  └─> Reads data in background thread
  └─> Invokes callbacks with received data

// Data parsing
reflection_data.cpp
  └─> Parses JSON into data structures
  └─> Simple custom parser (consider using nlohmann/json)

// UI rendering
ui/main_window.cpp
  └─> Renders ImGui interface
  └─> Handles user interaction
  └─> Displays reflection data
```

## Future Enhancements

- [ ] Save/load reflection snapshots
- [ ] Export to various formats (JSON, XML, CSV)
- [ ] Diff between multiple snapshots
- [ ] Search within members
- [ ] Syntax highlighting for signatures
- [ ] Dark/Light theme toggle
- [ ] Resizable panels
- [ ] GameObject hierarchy view
- [ ] Runtime field inspection
- [ ] Method invocation

## Contributing

Contributions are welcome! Areas for improvement:
- Better JSON parser (consider nlohmann/json)
- UI enhancements
- Additional filters and search options
- Export functionality
- Unit tests

## License

MIT License
