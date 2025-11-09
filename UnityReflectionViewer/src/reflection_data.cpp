#include "reflection_data.h"
#include <sstream>
#include <algorithm>

namespace UnityReflection {

// Simple JSON parser (basic implementation, consider using a library like nlohmann/json for production)
class SimpleJsonParser {
public:
    SimpleJsonParser(const std::string& json) : json_(json), pos_(0) {}

    bool ParseAssemblyData(AssemblyData& data) {
        SkipWhitespace();
        if (!Expect('{')) return false;

        while (pos_ < json_.size()) {
            SkipWhitespace();
            if (Peek() == '}') {
                pos_++;
                return true;
            }

            std::string key = ParseString();
            SkipWhitespace();
            if (!Expect(':')) return false;
            SkipWhitespace();

            if (key == "assemblyName") {
                data.assemblyName = ParseString();
            } else if (key == "timestamp") {
                data.timestamp = ParseString();
            } else if (key == "types") {
                if (!ParseTypesArray(data.types)) return false;
            } else {
                SkipValue();
            }

            SkipWhitespace();
            if (Peek() == ',') pos_++;
        }

        return true;
    }

private:
    const std::string& json_;
    size_t pos_;

    char Peek() {
        return pos_ < json_.size() ? json_[pos_] : '\0';
    }

    bool Expect(char c) {
        if (Peek() == c) {
            pos_++;
            return true;
        }
        return false;
    }

    void SkipWhitespace() {
        while (pos_ < json_.size() && (json_[pos_] == ' ' || json_[pos_] == '\n' ||
               json_[pos_] == '\r' || json_[pos_] == '\t')) {
            pos_++;
        }
    }

    std::string ParseString() {
        if (!Expect('"')) return "";

        std::string result;
        while (pos_ < json_.size() && json_[pos_] != '"') {
            if (json_[pos_] == '\\' && pos_ + 1 < json_.size()) {
                pos_++;
                switch (json_[pos_]) {
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    case '\\': result += '\\'; break;
                    case '"': result += '"'; break;
                    default: result += json_[pos_]; break;
                }
            } else {
                result += json_[pos_];
            }
            pos_++;
        }

        Expect('"');
        return result;
    }

    bool ParseBool() {
        if (json_.compare(pos_, 4, "true") == 0) {
            pos_ += 4;
            return true;
        } else if (json_.compare(pos_, 5, "false") == 0) {
            pos_ += 5;
            return false;
        }
        return false;
    }

    void SkipValue() {
        SkipWhitespace();
        char c = Peek();
        if (c == '"') {
            ParseString();
        } else if (c == '{') {
            SkipObject();
        } else if (c == '[') {
            SkipArray();
        } else if (c == 't' || c == 'f') {
            ParseBool();
        } else {
            while (pos_ < json_.size() && json_[pos_] != ',' && json_[pos_] != '}' && json_[pos_] != ']') {
                pos_++;
            }
        }
    }

    void SkipObject() {
        if (!Expect('{')) return;
        int depth = 1;
        while (pos_ < json_.size() && depth > 0) {
            if (json_[pos_] == '{') depth++;
            else if (json_[pos_] == '}') depth--;
            pos_++;
        }
    }

    void SkipArray() {
        if (!Expect('[')) return;
        int depth = 1;
        while (pos_ < json_.size() && depth > 0) {
            if (json_[pos_] == '[') depth++;
            else if (json_[pos_] == ']') depth--;
            pos_++;
        }
    }

    bool ParseTypesArray(std::vector<TypeInfo>& types) {
        if (!Expect('[')) return false;

        while (pos_ < json_.size()) {
            SkipWhitespace();
            if (Peek() == ']') {
                pos_++;
                return true;
            }

            TypeInfo type;
            if (!ParseTypeInfo(type)) return false;
            types.push_back(std::move(type));

            SkipWhitespace();
            if (Peek() == ',') pos_++;
        }

        return true;
    }

    bool ParseTypeInfo(TypeInfo& type) {
        if (!Expect('{')) return false;

        while (pos_ < json_.size()) {
            SkipWhitespace();
            if (Peek() == '}') {
                pos_++;
                return true;
            }

            std::string key = ParseString();
            SkipWhitespace();
            if (!Expect(':')) return false;
            SkipWhitespace();

            if (key == "name") type.name = ParseString();
            else if (key == "fullName") type.fullName = ParseString();
            else if (key == "namespace") type.namespaceName = ParseString();
            else if (key == "baseType") type.baseType = ParseString();
            else if (key == "isClass") type.isClass = ParseBool();
            else if (key == "isStruct") type.isStruct = ParseBool();
            else if (key == "isEnum") type.isEnum = ParseBool();
            else if (key == "isInterface") type.isInterface = ParseBool();
            else if (key == "fields") ParseFieldsArray(type.fields);
            else if (key == "methods") ParseMethodsArray(type.methods);
            else if (key == "properties") ParsePropertiesArray(type.properties);
            else SkipValue();

            SkipWhitespace();
            if (Peek() == ',') pos_++;
        }

        return true;
    }

    bool ParseFieldsArray(std::vector<FieldInfo>& fields) {
        if (!Expect('[')) return false;

        while (pos_ < json_.size()) {
            SkipWhitespace();
            if (Peek() == ']') {
                pos_++;
                return true;
            }

            FieldInfo field;
            if (!ParseFieldInfo(field)) return false;
            fields.push_back(std::move(field));

            SkipWhitespace();
            if (Peek() == ',') pos_++;
        }
        return true;
    }

    bool ParseFieldInfo(FieldInfo& field) {
        if (!Expect('{')) return false;

        while (pos_ < json_.size()) {
            SkipWhitespace();
            if (Peek() == '}') {
                pos_++;
                return true;
            }

            std::string key = ParseString();
            SkipWhitespace();
            if (!Expect(':')) return false;
            SkipWhitespace();

            if (key == "name") field.name = ParseString();
            else if (key == "fieldType") field.fieldType = ParseString();
            else if (key == "isPublic") field.isPublic = ParseBool();
            else if (key == "isStatic") field.isStatic = ParseBool();
            else if (key == "isReadOnly") field.isReadOnly = ParseBool();
            else SkipValue();

            SkipWhitespace();
            if (Peek() == ',') pos_++;
        }
        return true;
    }

    bool ParseMethodsArray(std::vector<MethodInfo>& methods) {
        if (!Expect('[')) return false;

        while (pos_ < json_.size()) {
            SkipWhitespace();
            if (Peek() == ']') {
                pos_++;
                return true;
            }

            MethodInfo method;
            if (!ParseMethodInfo(method)) return false;
            methods.push_back(std::move(method));

            SkipWhitespace();
            if (Peek() == ',') pos_++;
        }
        return true;
    }

    bool ParseMethodInfo(MethodInfo& method) {
        if (!Expect('{')) return false;

        while (pos_ < json_.size()) {
            SkipWhitespace();
            if (Peek() == '}') {
                pos_++;
                return true;
            }

            std::string key = ParseString();
            SkipWhitespace();
            if (!Expect(':')) return false;
            SkipWhitespace();

            if (key == "name") method.name = ParseString();
            else if (key == "returnType") method.returnType = ParseString();
            else if (key == "isPublic") method.isPublic = ParseBool();
            else if (key == "isStatic") method.isStatic = ParseBool();
            else if (key == "parameters") ParseParametersArray(method.parameters);
            else SkipValue();

            SkipWhitespace();
            if (Peek() == ',') pos_++;
        }
        return true;
    }

    bool ParseParametersArray(std::vector<ParameterInfo>& parameters) {
        if (!Expect('[')) return false;

        while (pos_ < json_.size()) {
            SkipWhitespace();
            if (Peek() == ']') {
                pos_++;
                return true;
            }

            ParameterInfo param;
            if (!ParseParameterInfo(param)) return false;
            parameters.push_back(std::move(param));

            SkipWhitespace();
            if (Peek() == ',') pos_++;
        }
        return true;
    }

    bool ParseParameterInfo(ParameterInfo& param) {
        if (!Expect('{')) return false;

        while (pos_ < json_.size()) {
            SkipWhitespace();
            if (Peek() == '}') {
                pos_++;
                return true;
            }

            std::string key = ParseString();
            SkipWhitespace();
            if (!Expect(':')) return false;
            SkipWhitespace();

            if (key == "name") param.name = ParseString();
            else if (key == "parameterType") param.parameterType = ParseString();
            else SkipValue();

            SkipWhitespace();
            if (Peek() == ',') pos_++;
        }
        return true;
    }

    bool ParsePropertiesArray(std::vector<PropertyInfo>& properties) {
        if (!Expect('[')) return false;

        while (pos_ < json_.size()) {
            SkipWhitespace();
            if (Peek() == ']') {
                pos_++;
                return true;
            }

            PropertyInfo prop;
            if (!ParsePropertyInfo(prop)) return false;
            properties.push_back(std::move(prop));

            SkipWhitespace();
            if (Peek() == ',') pos_++;
        }
        return true;
    }

    bool ParsePropertyInfo(PropertyInfo& prop) {
        if (!Expect('{')) return false;

        while (pos_ < json_.size()) {
            SkipWhitespace();
            if (Peek() == '}') {
                pos_++;
                return true;
            }

            std::string key = ParseString();
            SkipWhitespace();
            if (!Expect(':')) return false;
            SkipWhitespace();

            if (key == "name") prop.name = ParseString();
            else if (key == "propertyType") prop.propertyType = ParseString();
            else if (key == "canRead") prop.canRead = ParseBool();
            else if (key == "canWrite") prop.canWrite = ParseBool();
            else SkipValue();

            SkipWhitespace();
            if (Peek() == ',') pos_++;
        }
        return true;
    }
};

bool ParseAssemblyData(const std::string& json, AssemblyData& data) {
    SimpleJsonParser parser(json);
    return parser.ParseAssemblyData(data);
}

} // namespace UnityReflection
