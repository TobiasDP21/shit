# Unity Reflection Library & Viewer

A comprehensive solution for reflecting and viewing Unity game assemblies (Assembly-CSharp) from an external C++ application using ImGui.

## Overview

This project consists of two main components:

1. **UnityReflectionLib** - A C# library that runs inside Unity, uses reflection to extract all classes, methods, fields, and properties from Assembly-CSharp
2. **UnityReflectionViewer** - A C++ application with ImGui UI that receives and displays the reflection data

The two components communicate via Named Pipes for efficient IPC (Inter-Process Communication).

## Features

- **Complete Reflection**: Extracts all types, methods, fields, and properties from Assembly-CSharp
- **Real-time Data**: Uses IPC to send reflection data from Unity to the viewer
- **Rich UI**: Beautiful ImGui-based interface with filtering, search, and categorization
- **Cross-platform**: Works on Windows, Linux, and macOS
- **Type Information**: View classes, structs, enums, and interfaces with full metadata
- **Member Details**: See access modifiers, static/instance, return types, and parameters

## Project Structure

```
.
├── UnityReflectionLib/          # C# Unity library
│   ├── UnityReflectionLib.csproj
│   ├── AssemblyReflector.cs     # Core reflection logic
│   ├── ReflectionData.cs        # Data models
│   ├── IPCServer.cs             # Named pipe server
│   └── UnityReflectionManager.cs # Unity MonoBehaviour component
│
└── UnityReflectionViewer/       # C++ ImGui viewer
    ├── CMakeLists.txt
    ├── src/
    │   ├── main.cpp             # Application entry point
    │   ├── ipc_client.cpp/h     # Named pipe client
    │   ├── reflection_data.cpp/h # Data models & JSON parser
    │   └── ui/
    │       └── main_window.cpp/h # ImGui UI
    └── external/
        └── imgui/               # ImGui library (to be added)
```

## Getting Started

See the individual README files in each component for detailed setup instructions:

- [UnityReflectionLib Setup](./UnityReflectionLib/README.md)
- [UnityReflectionViewer Setup](./UnityReflectionViewer/README.md)

## Quick Start

### 1. Build the C# Library

```bash
cd UnityReflectionLib
dotnet build
```

### 2. Add to Unity Project

1. Copy the built DLL to your Unity project's `Assets/Plugins/` folder
2. Add the `UnityReflectionManager` component to a GameObject in your scene
3. The IPC server will start automatically when you play the scene

### 3. Build and Run the C++ Viewer

```bash
cd UnityReflectionViewer
mkdir build && cd build
cmake ..
make
./UnityReflectionViewer
```

### 4. Connect

1. Start the Unity game (Play mode)
2. The viewer will automatically connect and display the reflection data

## Usage

### In Unity

The `UnityReflectionManager` component provides:
- **Auto Start**: Automatically starts the IPC server when the scene loads
- **Context Menu**: Right-click the component for manual controls:
  - Start IPC Server
  - Stop IPC Server
  - Test Reflection

### In the Viewer

The ImGui viewer provides:
- **Type List**: Browse all types with color-coded icons
  - [C] Blue = Class
  - [S] Yellow = Struct
  - [E] Purple = Enum
  - [I] Green = Interface
- **Search**: Filter types by name or namespace
- **Type Filters**: Show only specific type categories
- **Member Tabs**: View Fields, Methods, and Properties
- **Details**: See full type information including base types

## How It Works

1. **Unity Side**:
   - Uses C# Reflection API to scan Assembly-CSharp
   - Extracts type information (classes, structs, enums, interfaces)
   - Serializes data to JSON
   - Sends via Named Pipe IPC

2. **Viewer Side**:
   - Connects to the Named Pipe
   - Receives and parses JSON data
   - Displays in an interactive ImGui interface
   - Auto-reconnects when Unity restarts

## Requirements

### C# Library
- .NET Framework 4.7.1+ or .NET Standard 2.0+
- Unity 2019.1 or later

### C++ Viewer
- C++17 compatible compiler
- CMake 3.15+
- GLFW3
- OpenGL 3.0+
- ImGui (included as submodule or manual download)

## Platform Support

| Platform | C# Library | C++ Viewer |
|----------|-----------|------------|
| Windows  | ✅        | ✅         |
| Linux    | ✅        | ✅         |
| macOS    | ✅        | ✅         |

## License

MIT License - Feel free to use in your projects!

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues.

## Troubleshooting

### Connection Issues

**Problem**: Viewer can't connect to Unity

**Solutions**:
- Ensure Unity is in Play mode
- Check that the `UnityReflectionManager` component is active
- Verify no firewall is blocking named pipes
- On Linux: Check `/tmp/UnityReflectionPipe` permissions

### Build Issues

**C# Build Fails**:
- Update `UNITY_PATH` environment variable
- Or manually set UnityEngine.dll paths in the .csproj

**C++ Build Fails**:
- Ensure GLFW3 is installed: `sudo apt install libglfw3-dev` (Linux)
- Download ImGui manually if needed: https://github.com/ocornut/imgui

## Future Enhancements

- [ ] Real-time GameObject hierarchy viewing
- [ ] Field value inspection
- [ ] Method invocation
- [ ] Save/load reflection snapshots
- [ ] Export to various formats (JSON, XML, CSV)
- [ ] Performance profiling integration

## Credits

- [Dear ImGui](https://github.com/ocornut/imgui) - UI framework
- [GLFW](https://www.glfw.org/) - Window and input handling
