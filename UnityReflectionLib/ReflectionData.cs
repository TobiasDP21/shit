using System;
using System.Collections.Generic;

namespace UnityReflectionLib
{
    [Serializable]
    public class TypeInfo
    {
        public string Name { get; set; } = string.Empty;
        public string FullName { get; set; } = string.Empty;
        public string Namespace { get; set; } = string.Empty;
        public string BaseType { get; set; } = string.Empty;
        public bool IsClass { get; set; }
        public bool IsStruct { get; set; }
        public bool IsEnum { get; set; }
        public bool IsInterface { get; set; }
        public List<FieldInfo> Fields { get; set; } = new List<FieldInfo>();
        public List<MethodInfo> Methods { get; set; } = new List<MethodInfo>();
        public List<PropertyInfo> Properties { get; set; } = new List<PropertyInfo>();
    }

    [Serializable]
    public class FieldInfo
    {
        public string Name { get; set; } = string.Empty;
        public string FieldType { get; set; } = string.Empty;
        public bool IsPublic { get; set; }
        public bool IsStatic { get; set; }
        public bool IsReadOnly { get; set; }
    }

    [Serializable]
    public class MethodInfo
    {
        public string Name { get; set; } = string.Empty;
        public string ReturnType { get; set; } = string.Empty;
        public bool IsPublic { get; set; }
        public bool IsStatic { get; set; }
        public List<ParameterInfo> Parameters { get; set; } = new List<ParameterInfo>();
    }

    [Serializable]
    public class ParameterInfo
    {
        public string Name { get; set; } = string.Empty;
        public string ParameterType { get; set; } = string.Empty;
    }

    [Serializable]
    public class PropertyInfo
    {
        public string Name { get; set; } = string.Empty;
        public string PropertyType { get; set; } = string.Empty;
        public bool CanRead { get; set; }
        public bool CanWrite { get; set; }
    }

    [Serializable]
    public class AssemblyData
    {
        public string AssemblyName { get; set; } = string.Empty;
        public List<TypeInfo> Types { get; set; } = new List<TypeInfo>();
        public DateTime Timestamp { get; set; }
    }
}
