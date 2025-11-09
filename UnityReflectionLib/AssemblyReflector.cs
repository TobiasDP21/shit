using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

namespace UnityReflectionLib
{
    public class AssemblyReflector
    {
        private const BindingFlags DefaultFlags = BindingFlags.Public | BindingFlags.NonPublic |
                                                   BindingFlags.Instance | BindingFlags.Static |
                                                   BindingFlags.DeclaredOnly;

        public static AssemblyData ReflectAssemblyCSharp()
        {
            try
            {
                var assembly = Assembly.Load("Assembly-CSharp");
                return ReflectAssembly(assembly);
            }
            catch (Exception ex)
            {
                UnityEngine.Debug.LogError($"Failed to load Assembly-CSharp: {ex.Message}");
                return new AssemblyData { AssemblyName = "Assembly-CSharp (Failed to load)" };
            }
        }

        public static AssemblyData ReflectAssembly(Assembly assembly)
        {
            var data = new AssemblyData
            {
                AssemblyName = assembly.GetName().Name ?? "Unknown",
                Timestamp = DateTime.Now
            };

            try
            {
                var types = assembly.GetTypes();
                foreach (var type in types)
                {
                    try
                    {
                        var typeInfo = ExtractTypeInfo(type);
                        data.Types.Add(typeInfo);
                    }
                    catch (Exception ex)
                    {
                        UnityEngine.Debug.LogWarning($"Failed to reflect type {type.FullName}: {ex.Message}");
                    }
                }
            }
            catch (ReflectionTypeLoadException ex)
            {
                UnityEngine.Debug.LogError($"ReflectionTypeLoadException: {ex.Message}");
                if (ex.Types != null)
                {
                    foreach (var type in ex.Types.Where(t => t != null))
                    {
                        try
                        {
                            var typeInfo = ExtractTypeInfo(type!);
                            data.Types.Add(typeInfo);
                        }
                        catch { }
                    }
                }
            }

            return data;
        }

        private static TypeInfo ExtractTypeInfo(Type type)
        {
            var typeInfo = new TypeInfo
            {
                Name = type.Name,
                FullName = type.FullName ?? type.Name,
                Namespace = type.Namespace ?? string.Empty,
                BaseType = type.BaseType?.FullName ?? string.Empty,
                IsClass = type.IsClass,
                IsStruct = type.IsValueType && !type.IsEnum,
                IsEnum = type.IsEnum,
                IsInterface = type.IsInterface
            };

            // Extract fields
            var fields = type.GetFields(DefaultFlags);
            foreach (var field in fields)
            {
                typeInfo.Fields.Add(new FieldInfo
                {
                    Name = field.Name,
                    FieldType = GetTypeName(field.FieldType),
                    IsPublic = field.IsPublic,
                    IsStatic = field.IsStatic,
                    IsReadOnly = field.IsInitOnly
                });
            }

            // Extract methods
            var methods = type.GetMethods(DefaultFlags);
            foreach (var method in methods)
            {
                // Skip property accessors and special names
                if (method.IsSpecialName) continue;

                var methodInfo = new MethodInfo
                {
                    Name = method.Name,
                    ReturnType = GetTypeName(method.ReturnType),
                    IsPublic = method.IsPublic,
                    IsStatic = method.IsStatic
                };

                var parameters = method.GetParameters();
                foreach (var param in parameters)
                {
                    methodInfo.Parameters.Add(new ParameterInfo
                    {
                        Name = param.Name ?? "param",
                        ParameterType = GetTypeName(param.ParameterType)
                    });
                }

                typeInfo.Methods.Add(methodInfo);
            }

            // Extract properties
            var properties = type.GetProperties(DefaultFlags);
            foreach (var property in properties)
            {
                typeInfo.Properties.Add(new PropertyInfo
                {
                    Name = property.Name,
                    PropertyType = GetTypeName(property.PropertyType),
                    CanRead = property.CanRead,
                    CanWrite = property.CanWrite
                });
            }

            return typeInfo;
        }

        private static string GetTypeName(Type type)
        {
            if (type == null) return "void";

            if (type.IsGenericType)
            {
                var genericTypeName = type.GetGenericTypeDefinition().Name;
                var tickIndex = genericTypeName.IndexOf('`');
                if (tickIndex > 0)
                    genericTypeName = genericTypeName.Substring(0, tickIndex);

                var genericArgs = type.GetGenericArguments();
                var argNames = string.Join(", ", genericArgs.Select(GetTypeName));
                return $"{genericTypeName}<{argNames}>";
            }

            return type.FullName ?? type.Name;
        }
    }
}
