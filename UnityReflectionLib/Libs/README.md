# Libraries Directory

This directory should contain the required DLL references for building the MelonLoader mod.

## Required Files

Place the following DLL files in this directory:

### 1. MelonLoader.dll
- **Source**: From your MelonLoader installation
- **Location**: `<Game>/MelonLoader/MelonLoader.dll`
- **Purpose**: Core MelonLoader API

### 2. UnityEngine.dll
- **Source**: From the target Unity game's managed folder
- **Location**: `<Game>/<GameName>_Data/Managed/UnityEngine.dll`
- **Purpose**: Unity Engine core library

### 3. UnityEngine.CoreModule.dll
- **Source**: From the target Unity game's managed folder
- **Location**: `<Game>/<GameName>_Data/Managed/UnityEngine.CoreModule.dll`
- **Purpose**: Unity Engine core module

## How to Get These Files

### For a Specific Game:

1. **Install MelonLoader** on the target Unity game
   - Download from: https://github.com/LavaGang/MelonLoader/releases
   - Run the installer on your game executable

2. **Locate the Game's Managed Folder**:
   - Find your game installation directory
   - Navigate to `<GameName>_Data/Managed/`

3. **Copy Required DLLs**:
   ```bash
   # From game directory:
   cp MelonLoader/MelonLoader.dll /path/to/UnityReflectionLib/Libs/
   cp <GameName>_Data/Managed/UnityEngine.dll /path/to/UnityReflectionLib/Libs/
   cp <GameName>_Data/Managed/UnityEngine.CoreModule.dll /path/to/UnityReflectionLib/Libs/
   ```

## Directory Structure After Setup

```
Libs/
├── README.md (this file)
├── MelonLoader.dll
├── UnityEngine.dll
└── UnityEngine.CoreModule.dll
```

## .gitignore

These DLL files should not be committed to the repository (they are game-specific and subject to licensing). They are already excluded via `.gitignore`.
