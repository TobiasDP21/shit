# Quick Setup Guide

Follow these steps to get the Unity Reflection Viewer up and running.

## Step 1: Clone ImGui (C++ Viewer)

The C++ viewer requires Dear ImGui library:

```bash
cd UnityReflectionViewer/external
git clone https://github.com/ocornut/imgui.git
cd ../..
```

## Step 2: Build the C# Unity Library

### Option A: Build as DLL
```bash
cd UnityReflectionLib
dotnet build -c Release
```

Then copy `bin/Release/net471/UnityReflectionLib.dll` to your Unity project's `Assets/Plugins/` folder.

### Option B: Use as Source
Simply copy all `.cs` files from `UnityReflectionLib/` into your Unity project's `Assets/Scripts/` folder.

## Step 3: Setup in Unity

1. Open your Unity project
2. If using DLL: Ensure the DLL is in `Assets/Plugins/`
3. Create an empty GameObject in your scene
4. Add the `UnityReflectionManager` component to it
5. Ensure "Auto Start" is checked in the inspector

## Step 4: Build the C++ Viewer

### Windows
```bash
cd UnityReflectionViewer
build.bat
```

### Linux/macOS
```bash
cd UnityReflectionViewer
./build.sh
```

Or manually:
```bash
cd UnityReflectionViewer
mkdir build && cd build
cmake ..
make -j4
```

## Step 5: Run Everything

1. **Start the C++ Viewer**:
   - Windows: `build/Release/UnityReflectionViewer.exe`
   - Linux/Mac: `build/UnityReflectionViewer`

2. **Start Unity**:
   - Press Play in the Unity Editor
   - The viewer will automatically connect and display your Assembly-CSharp data

## Troubleshooting Quick Fixes

### Can't build C++ viewer
- **Missing GLFW**:
  - Windows: `vcpkg install glfw3`
  - Linux: `sudo apt install libglfw3-dev`
  - macOS: `brew install glfw`

### Viewer can't connect to Unity
- Ensure Unity is in Play mode
- Check Unity console for "IPC Server started" message
- Check firewall isn't blocking named pipes

### C# build fails
- Set `UNITY_PATH` environment variable to your Unity installation
- Or comment out the UnityEngine references in the .csproj and use the source files directly

## Dependencies Summary

### C# Library
- .NET Framework 4.7.1+ (or .NET Standard 2.0+)
- Unity 2019.1+

### C++ Viewer
- CMake 3.15+
- C++17 compiler
- GLFW3
- OpenGL 3.0+
- ImGui (clone from GitHub)

## Next Steps

Once everything is running:

1. Browse types in the left panel
2. Click on a type to see its details
3. Use the search box to filter types
4. Check out the Fields, Methods, and Properties tabs

For more information, see the detailed READMEs:
- [Main README](README.md)
- [C# Library README](UnityReflectionLib/README.md)
- [C++ Viewer README](UnityReflectionViewer/README.md)
