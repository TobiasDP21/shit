# Quick Setup Guide - MelonLoader Mod & Viewer

Get up and running in 5 minutes!

## Prerequisites Check

Before starting, make sure you have:

- [ ] A MelonLoader-compatible Unity game (Mono, not IL2CPP)
- [ ] MelonLoader installed on that game
- [ ] .NET 6.0 SDK
- [ ] C++17 compiler (GCC, Clang, or MSVC)
- [ ] CMake 3.15+
- [ ] GLFW3 library

## Step 1: Prepare Required DLLs

Navigate to your MelonLoader-enabled game and copy these DLLs:

```bash
cd UnityReflectionLib/Libs

# Copy from your game directory
# Windows example:
copy "C:\Games\YourGame\MelonLoader\MelonLoader.dll" .
copy "C:\Games\YourGame\YourGame_Data\Managed\UnityEngine.dll" .
copy "C:\Games\YourGame\YourGame_Data\Managed\UnityEngine.CoreModule.dll" .

# Linux/Mac example:
cp ~/Games/YourGame/MelonLoader/MelonLoader.dll .
cp ~/Games/YourGame/YourGame_Data/Managed/UnityEngine.dll .
cp ~/Games/YourGame/YourGame_Data/Managed/UnityEngine.CoreModule.dll .
```

**Important**: Replace `YourGame` with your actual game name!

## Step 2: Build the MelonLoader Mod

```bash
cd UnityReflectionLib
dotnet build -c Release
```

Expected output:
```
Build succeeded.
    0 Warning(s)
    0 Error(s)
```

The compiled DLL will be at: `bin/Release/net6.0/UnityReflectionMod.dll`

## Step 3: Install the Mod to Your Game

```bash
# Windows
copy bin\Release\net6.0\UnityReflectionMod.dll "C:\Games\YourGame\Mods\"

# Linux/Mac
cp bin/Release/net6.0/UnityReflectionMod.dll ~/Games/YourGame/Mods/
```

## Step 4: Setup the C++ Viewer

### Download ImGui

```bash
cd UnityReflectionViewer/external
git clone https://github.com/ocornut/imgui.git
cd ../..
```

### Install GLFW (if not already installed)

**Windows (vcpkg)**:
```powershell
vcpkg install glfw3
```

**Linux (Debian/Ubuntu)**:
```bash
sudo apt-get install libglfw3-dev libgl1-mesa-dev
```

**macOS (Homebrew)**:
```bash
brew install glfw
```

## Step 5: Build the C++ Viewer

```bash
cd UnityReflectionViewer

# Option 1: Use build script (Linux/Mac)
chmod +x build.sh
./build.sh

# Option 2: Use build script (Windows)
build.bat

# Option 3: Manual build
mkdir build && cd build
cmake ..
make  # or: cmake --build . --config Release (Windows)
```

## Step 6: Run Everything!

### Terminal 1: Start the Viewer

```bash
cd UnityReflectionViewer/build

# Linux/Mac
./UnityReflectionViewer

# Windows
.\Release\UnityReflectionViewer.exe
```

You should see:
```
Starting IPC listener...
Waiting for Unity to connect...
Attempting to connect to Unity...
```

### Terminal 2: Launch Your Game

Start your MelonLoader-enabled Unity game normally.

Watch the MelonLoader console for:
```
[UnityReflectionMod] Unity Reflection Mod initialized!
[UnityReflectionMod] Starting IPC server...
[UnityReflectionMod] [IPC] IPC Server started on pipe: UnityReflectionPipe
[UnityReflectionMod] [IPC] Waiting for client connection...
[UnityReflectionMod] [IPC] Client connected!
[UnityReflectionMod] [IPC] Sent XXXXX bytes to client
```

The viewer window will automatically populate with data!

## Verify It's Working

In the viewer, you should see:

1. **Top bar**: Assembly name and type counts
2. **Left panel**: List of types (classes, structs, enums, interfaces)
3. **Right panel**: Type details when you select one

Try this:
1. Type something in the search box (e.g., "Player")
2. Click on a type in the left panel
3. Browse the Fields, Methods, and Properties tabs

## Common Issues & Quick Fixes

### ❌ "Failed to load Assembly-CSharp"

**Fix**: The game might be using IL2CPP. This mod only works with Mono games.

**Check**: Look for `GameAssembly.dll` in the game folder. If it exists, it's IL2CPP.

### ❌ "MelonLoader.dll not found"

**Fix**: Make sure you copied the DLLs to the `Libs/` folder correctly.

### ❌ "Failed to connect to pipe"

**Fixes**:
1. Make sure the game is running
2. Check that MelonLoader loaded the mod (see console)
3. Try running both as administrator (Windows)
4. Check firewall settings

### ❌ "GLFW not found" (during build)

**Fix**: Install GLFW using your package manager (see Step 4)

### ❌ "ImGui not found" (during build)

**Fix**: Make sure you cloned ImGui into `external/imgui/`

### ❌ Viewer shows no types

**Fixes**:
1. Check MelonLoader console for errors
2. Make sure Assembly-CSharp exists in the game
3. Try a different game to verify the mod works

## File Locations Quick Reference

```
YourGame/
├── MelonLoader/
│   └── MelonLoader.dll              # Copy to Libs/
├── Mods/
│   └── UnityReflectionMod.dll       # Install mod here
└── YourGame_Data/
    └── Managed/
        ├── UnityEngine.dll          # Copy to Libs/
        └── UnityEngine.CoreModule.dll  # Copy to Libs/
```

## Testing With a Sample Game

If you want to test before using on your main game:

1. **VRChat**: Free on Steam, very mod-friendly
2. **Beat Saber**: Popular rhythm game with active modding community
3. **Boneworks**: Physics-based game, works great

All of these work with MelonLoader.

## Next Steps

Once everything is working:

1. **Explore the UI**:
   - Use search to find specific classes
   - Filter by type category (Classes, Structs, etc.)
   - Click on types to see their members

2. **Try Advanced Features**:
   - Modify `ReflectionMod.cs` to reflect other assemblies
   - Add custom filtering in `AssemblyReflector.cs`
   - Export data for further analysis

3. **Read the Docs**:
   - [MelonLoader Mod README](UnityReflectionLib/README.md)
   - [C++ Viewer README](UnityReflectionViewer/README.md)

## Getting Help

If you're stuck:

1. Check the detailed READMEs in each component folder
2. Look at MelonLoader console for error messages
3. Verify all prerequisites are installed
4. Make sure your game is Mono (not IL2CPP)

## Success Checklist

- [ ] Required DLLs copied to `Libs/`
- [ ] Mod builds without errors
- [ ] Mod installed to game's `Mods/` folder
- [ ] ImGui cloned to `external/imgui/`
- [ ] GLFW installed
- [ ] Viewer builds successfully
- [ ] Viewer starts and waits for connection
- [ ] Game launches with MelonLoader
- [ ] Mod shows in MelonLoader console
- [ ] Viewer connects and shows data
- [ ] Can search and browse types

If all checkboxes are checked, you're ready to go! 🎉

---

**Estimated setup time**: 5-10 minutes (after prerequisites are installed)
