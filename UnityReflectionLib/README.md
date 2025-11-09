# Unity Reflection MelonLoader Mod

A MelonLoader mod that extracts and transmits Assembly-CSharp reflection data from any Unity game to an external viewer application.

## What is This?

This is a **MelonLoader mod** that hooks into Unity games and uses reflection to extract information about:
- All classes, structs, enums, and interfaces in Assembly-CSharp
- All fields (public and private)
- All methods with full signatures
- All properties

The data is then continuously streamed to an external C++ viewer application via Named Pipes for real-time analysis.

## Use Cases

- Game modding and reverse engineering
- Understanding game architecture
- Finding hooks for mod development
- Learning how games are structured
- Security research and analysis

## Requirements

- .NET 6.0 SDK
- MelonLoader installed on target Unity game
- Target Unity game (any game that uses MelonLoader)

## Installation

### Step 1: Get Required DLLs

Copy the following files to the `Libs/` directory:

```bash
# From your MelonLoader-enabled game:
cp <GamePath>/MelonLoader/MelonLoader.dll Libs/
cp <GamePath>/<GameName>_Data/Managed/UnityEngine.dll Libs/
cp <GamePath>/<GameName>_Data/Managed/UnityEngine.CoreModule.dll Libs/
```

See [Libs/README.md](Libs/README.md) for detailed instructions.

### Step 2: Build the Mod

```bash
dotnet build -c Release
```

The compiled mod will be in `bin/Release/net6.0/UnityReflectionMod.dll`

### Step 3: Install to Game

Copy the built DLL to your game's Mods folder:

```bash
cp bin/Release/net6.0/UnityReflectionMod.dll "<GamePath>/Mods/"
```

## Usage

### Starting the Mod

1. **Launch the External Viewer** (C++ application)
   - The viewer will wait for connection

2. **Launch the Game**
   - MelonLoader will load the mod automatically
   - Check the console for: `"Unity Reflection Mod initialized!"`
   - The mod will start the IPC server

3. **Automatic Connection**
   - The viewer will connect automatically
   - Assembly-CSharp data will be transmitted
   - Browse the data in the ImGui interface

### Console Output

When the mod loads successfully, you'll see:

```
[UnityReflectionMod] Unity Reflection Mod initialized!
[UnityReflectionMod] Starting IPC server...
[UnityReflectionMod] [IPC] IPC Server started on pipe: UnityReflectionPipe
[UnityReflectionMod] [IPC] Waiting for client connection...
[UnityReflectionMod] [IPC] Client connected! Starting live data stream...
[UnityReflectionMod] [IPC] Sent update: XXXXX bytes (XXX types)
[UnityReflectionMod] [IPC] Sent update: XXXXX bytes (XXX types)
... (continues every 1 second by default)
```

When viewer sends commands:
```
[UnityReflectionMod] [IPC] Received command: INTERVAL:500
[UnityReflectionMod] [IPC] Update interval set to 500ms
```

## Features

### Reflection Capabilities

- **Complete Type Extraction**: All types from Assembly-CSharp
- **Deep Introspection**: Fields, methods, properties with full metadata
- **Access Modifiers**: Public/private/protected detection
- **Static/Instance**: Distinguishes between static and instance members
- **Generic Types**: Properly handles generic types and parameters
- **Inheritance**: Captures base type information

### IPC Communication

- **Named Pipes**: Fast, reliable inter-process communication
- **Bidirectional**: Two-way communication for commands and data
- **Live Streaming**: Continuous data updates (default: 1 second interval)
- **Command Protocol**: Viewer can control update rate and request refreshes
- **JSON Format**: Human-readable data transmission
- **Auto-reconnect**: Viewer reconnects when game restarts
- **Background Thread**: Doesn't block game execution

## Configuration

### Update Interval

The default update interval is 1000ms (1 second). You can change it programmatically:

```csharp
ipcServer.UpdateInterval = 500; // Update every 500ms
```

Or send a command from the viewer:
```
INTERVAL:500
```

Minimum interval: 100ms

### Command Protocol

The viewer can send text commands to control the mod:

| Command | Description | Example |
|---------|-------------|---------|
| `INTERVAL:ms` | Set update interval in milliseconds | `INTERVAL:500` |
| `REFRESH` | Force immediate data refresh | `REFRESH` |

Commands are sent as UTF-8 strings over the bidirectional Named Pipe.

### Changing the Pipe Name

Edit `IPCServer.cs`:

```csharp
private const string PipeName = "UnityReflectionPipe"; // Change this
```

### Enabling/Disabling Auto-Start

The mod starts automatically. To disable, comment out the server start in `ReflectionMod.cs`:

```csharp
public override void OnInitializeMelon()
{
    // ipcServer.Start(); // Commented out
}
```

## API Reference

### ReflectionMod

Main MelonLoader mod class.

**Lifecycle Events**:
- `OnInitializeMelon()` - Called when mod loads
- `OnDeinitializeMelon()` - Called when mod unloads
- `OnSceneWasLoaded(int buildIndex, string sceneName)` - Called on scene load

### AssemblyReflector

Static class for performing reflection.

**Methods**:
- `ReflectAssemblyCSharp()` - Reflects Assembly-CSharp
- `ReflectAssembly(Assembly assembly)` - Reflects any assembly

### IPCServer

Named pipe server for IPC with live streaming capabilities.

**Properties**:
- `UpdateInterval` - Get/set update interval in milliseconds (min: 100ms)

**Methods**:
- `Start()` - Start the server and begin streaming
- `Stop()` - Stop the server

**Events**:
- `OnLog` - Logging events
- `OnError` - Error events

## Data Model

### AssemblyData
```csharp
class AssemblyData {
    string AssemblyName;
    List<TypeInfo> Types;
    DateTime Timestamp;
}
```

### TypeInfo
```csharp
class TypeInfo {
    string Name, FullName, Namespace, BaseType;
    bool IsClass, IsStruct, IsEnum, IsInterface;
    List<FieldInfo> Fields;
    List<MethodInfo> Methods;
    List<PropertyInfo> Properties;
}
```

## Troubleshooting

### Mod Doesn't Load

**Check**:
1. MelonLoader is installed correctly
2. DLL is in the `Mods/` folder
3. MelonLoader console for errors
4. .NET 6.0 runtime is installed

### IPC Connection Failed

**Check**:
1. External viewer is running
2. Pipe name matches in both applications
3. No firewall blocking local IPC
4. On Linux: `/tmp/UnityReflectionPipe` permissions

### Build Errors

**Missing DLLs**:
- Ensure all required DLLs are in `Libs/` folder
- Check paths in `.csproj` file

**Wrong .NET Version**:
- Install .NET 6.0 SDK
- Or change `TargetFramework` in `.csproj` to match your installation

### No Types Found

**Possible Causes**:
- Game uses IL2CPP (not supported, mod works with Mono only)
- Assembly-CSharp is empty or doesn't exist
- Game uses different assembly names

## Performance

- **Reflection Time**: 1-3 seconds for large assemblies (1000+ types)
- **Memory Usage**: ~50-100 MB additional
- **Game Impact**: Minimal, runs on background thread
- **Data Size**: Varies by game, typically 1-50 MB JSON

## Platform Support

- ✅ Windows (Named Pipes)
- ✅ Linux (Unix Domain Sockets - requires minor code changes)
- ⚠️ IL2CPP Games (Not supported, Mono only)

## Security & Legal

### Security Notes

- This mod exposes internal game structure
- Only use on games you own or have permission to mod
- The IPC server accepts local connections only
- No external network communication

### Legal Considerations

- Respect game EULAs and terms of service
- Some games prohibit modding or reverse engineering
- Use for educational and authorized research only
- Don't use for piracy or cheating

## Advanced Usage

### Reflecting Other Assemblies

Modify `ReflectionMod.cs` to reflect other assemblies:

```csharp
public override void OnInitializeMelon()
{
    // Reflect specific assembly
    var assembly = Assembly.Load("Assembly-CSharp-firstpass");
    var data = AssemblyReflector.ReflectAssembly(assembly);

    // Or reflect all loaded assemblies
    foreach (var asm in AppDomain.CurrentDomain.GetAssemblies())
    {
        LoggerInstance.Msg($"Found assembly: {asm.GetName().Name}");
    }
}
```

### Custom Data Filtering

Add filtering in `AssemblyReflector.cs`:

```csharp
// Only reflect public types
var types = assembly.GetTypes().Where(t => t.IsPublic);
```

## Building for Different Games

Each Unity game may use different Unity versions. If you encounter version mismatches:

1. Copy DLLs from the specific game
2. Rebuild the mod
3. The mod is now tailored to that game's Unity version

## Example Games

This mod works with any MelonLoader-compatible Unity game:

- VRChat
- Boneworks
- Beat Saber
- Rust
- Among Us
- Many more...

## Contributing

Contributions welcome! Areas for improvement:
- IL2CPP support via Unhollower
- Additional assembly reflection
- Performance optimizations
- Better error handling

## License

MIT License - Use freely for educational and research purposes
