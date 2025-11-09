# Unity Reflection Library

C# library for Unity that uses reflection to extract and transmit Assembly-CSharp information to external applications.

## Features

- Reflect all types from Assembly-CSharp assembly
- Extract classes, structs, enums, and interfaces
- Get all fields, methods, and properties with full metadata
- IPC server using Named Pipes for external communication
- Simple Unity MonoBehaviour component for easy integration

## Installation

### Option 1: Build from Source

1. Build the library:
```bash
cd UnityReflectionLib
dotnet build -c Release
```

2. Copy the DLL to your Unity project:
```bash
cp bin/Release/net471/UnityReflectionLib.dll /path/to/your/unity/project/Assets/Plugins/
```

### Option 2: Add to Unity as Source

Simply copy all `.cs` files into your Unity project's `Assets/Scripts/` folder.

## Setup in Unity

1. **Add the Component**:
   - Create an empty GameObject in your scene
   - Add the `UnityReflectionManager` component
   - The component will auto-start by default

2. **Configure** (Optional):
   - Uncheck "Auto Start" if you want manual control
   - Use context menu (right-click component) to:
     - Start/Stop IPC Server manually
     - Test reflection functionality

## Usage

### Basic Usage

The simplest way is to use the provided MonoBehaviour:

```csharp
// Add this component to a GameObject
public class MyGameObject : MonoBehaviour
{
    void Start()
    {
        // The UnityReflectionManager handles everything automatically
        gameObject.AddComponent<UnityReflectionManager>();
    }
}
```

### Advanced Usage

For more control, use the API directly:

```csharp
using UnityReflectionLib;

// Reflect Assembly-CSharp
var assemblyData = AssemblyReflector.ReflectAssemblyCSharp();

Debug.Log($"Found {assemblyData.Types.Count} types");

// Iterate through types
foreach (var type in assemblyData.Types)
{
    Debug.Log($"Type: {type.FullName}");
    Debug.Log($"  Fields: {type.Fields.Count}");
    Debug.Log($"  Methods: {type.Methods.Count}");
    Debug.Log($"  Properties: {type.Properties.Count}");
}

// Start IPC server manually
var ipcServer = new IPCServer();
ipcServer.OnLog += (msg) => Debug.Log(msg);
ipcServer.OnError += (msg) => Debug.LogError(msg);
ipcServer.Start();

// Later, stop it
ipcServer.Stop();
```

### Reflect Custom Assemblies

You can also reflect other assemblies:

```csharp
using System.Reflection;

var assembly = Assembly.Load("MyCustomAssembly");
var data = AssemblyReflector.ReflectAssembly(assembly);
```

## API Reference

### AssemblyReflector

Static class for performing reflection operations.

**Methods**:
- `ReflectAssemblyCSharp()` - Reflects the Assembly-CSharp assembly
- `ReflectAssembly(Assembly assembly)` - Reflects any assembly

### IPCServer

Named pipe server for sending reflection data to external applications.

**Methods**:
- `Start()` - Start the IPC server
- `Stop()` - Stop the IPC server

**Events**:
- `OnLog` - Logging messages
- `OnError` - Error messages

**Configuration**:
- Pipe Name: `UnityReflectionPipe`

### UnityReflectionManager

MonoBehaviour component for easy integration.

**Inspector Options**:
- `Auto Start` - Start IPC server on Awake

**Context Menu**:
- `Start IPC Server` - Manually start the server
- `Stop IPC Server` - Manually stop the server
- `Test Reflection` - Test reflection and log results

## Data Model

### AssemblyData
```csharp
class AssemblyData
{
    string AssemblyName;
    List<TypeInfo> Types;
    DateTime Timestamp;
}
```

### TypeInfo
```csharp
class TypeInfo
{
    string Name;
    string FullName;
    string Namespace;
    string BaseType;
    bool IsClass;
    bool IsStruct;
    bool IsEnum;
    bool IsInterface;
    List<FieldInfo> Fields;
    List<MethodInfo> Methods;
    List<PropertyInfo> Properties;
}
```

### FieldInfo
```csharp
class FieldInfo
{
    string Name;
    string FieldType;
    bool IsPublic;
    bool IsStatic;
    bool IsReadOnly;
}
```

### MethodInfo
```csharp
class MethodInfo
{
    string Name;
    string ReturnType;
    bool IsPublic;
    bool IsStatic;
    List<ParameterInfo> Parameters;
}
```

### PropertyInfo
```csharp
class PropertyInfo
{
    string Name;
    string PropertyType;
    bool CanRead;
    bool CanWrite;
}
```

## Requirements

- Unity 2019.1 or later
- .NET Framework 4.7.1 or .NET Standard 2.0

## Platform Support

- ✅ Windows
- ✅ Linux
- ✅ macOS

## Performance Considerations

- Reflection is performed once when the IPC server receives a connection
- The data is serialized to JSON and sent over the pipe
- For large projects (1000+ types), expect 1-2 seconds for full reflection
- The IPC server runs on a background thread and doesn't block the game

## Troubleshooting

### "Failed to load Assembly-CSharp"

**Cause**: Assembly-CSharp isn't available yet

**Solution**: Ensure reflection happens after the assembly is loaded (e.g., in Start, not Awake)

### Named Pipe Connection Issues

**Windows**: Check Windows Firewall settings

**Linux/Mac**: Verify `/tmp/UnityReflectionPipe` permissions

### Missing Types

**Cause**: Code stripping in build

**Solution**: Disable code stripping in Player Settings or use `link.xml` to preserve types

## Security Notes

- This library uses reflection to access all types in your assembly
- Only use in development builds, not production
- The IPC server exposes your code structure to any local process
- Consider adding authentication if needed

## Examples

See the `UnityReflectionManager` component for a complete working example.

## License

MIT License
