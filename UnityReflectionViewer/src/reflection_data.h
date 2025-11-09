#pragma once

#include <string>
#include <vector>

namespace UnityReflection {

struct ParameterInfo {
    std::string name;
    std::string parameterType;
};

struct MethodInfo {
    std::string name;
    std::string returnType;
    bool isPublic = false;
    bool isStatic = false;
    std::vector<ParameterInfo> parameters;
};

struct FieldInfo {
    std::string name;
    std::string fieldType;
    bool isPublic = false;
    bool isStatic = false;
    bool isReadOnly = false;
};

struct PropertyInfo {
    std::string name;
    std::string propertyType;
    bool canRead = false;
    bool canWrite = false;
};

struct TypeInfo {
    std::string name;
    std::string fullName;
    std::string namespaceName;
    std::string baseType;
    bool isClass = false;
    bool isStruct = false;
    bool isEnum = false;
    bool isInterface = false;
    std::vector<FieldInfo> fields;
    std::vector<MethodInfo> methods;
    std::vector<PropertyInfo> properties;
};

struct AssemblyData {
    std::string assemblyName;
    std::string timestamp;
    std::vector<TypeInfo> types;

    void Clear() {
        assemblyName.clear();
        timestamp.clear();
        types.clear();
    }
};

// JSON parsing
bool ParseAssemblyData(const std::string& json, AssemblyData& data);

} // namespace UnityReflection
