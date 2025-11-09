#include "main_window.h"
#include <imgui.h>
#include <algorithm>
#include <cstring>

namespace UnityReflection {
namespace UI {

MainWindow::MainWindow() {
}

MainWindow::~MainWindow() {
}

void MainWindow::SetAssemblyData(const AssemblyData& data) {
    assemblyData_ = data;
    selectedTypeIndex_ = -1;

    // Calculate stats
    totalClasses_ = 0;
    totalStructs_ = 0;
    totalEnums_ = 0;
    totalInterfaces_ = 0;

    for (const auto& type : assemblyData_.types) {
        if (type.isClass) totalClasses_++;
        if (type.isStruct) totalStructs_++;
        if (type.isEnum) totalEnums_++;
        if (type.isInterface) totalInterfaces_++;
    }
}

void MainWindow::Render() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Unity Reflection Viewer", nullptr, ImGuiWindowFlags_MenuBar)) {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit")) {
                    // Handle exit
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Show Public Only", nullptr, &showPublicOnly_);
                ImGui::MenuItem("Show Inherited Members", nullptr, &showInheritedMembers_);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        RenderConnectionStatus();

        // Main layout: Type list on left, details on right
        ImGui::BeginChild("TypeList", ImVec2(400, 0), true);
        RenderTypeList();
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("TypeDetails", ImVec2(0, 0), true);
        RenderTypeDetails();
        ImGui::EndChild();
    }
    ImGui::End();
}

void MainWindow::RenderConnectionStatus() {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
    ImGui::Text("Assembly: %s", assemblyData_.assemblyName.c_str());
    ImGui::PopStyleColor();

    ImGui::SameLine();
    ImGui::Text("| Types: %zu | Classes: %d | Structs: %d | Enums: %d | Interfaces: %d",
                assemblyData_.types.size(), totalClasses_, totalStructs_, totalEnums_, totalInterfaces_);

    if (!assemblyData_.timestamp.empty()) {
        ImGui::SameLine();
        ImGui::Text("| Last update: %s", assemblyData_.timestamp.c_str());
    }

    ImGui::Separator();
}

void MainWindow::RenderTypeList() {
    ImGui::Text("Types");
    ImGui::Separator();

    // Search bar
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##search", "Search types...", searchBuffer_, sizeof(searchBuffer_));

    ImGui::Separator();

    // Filter buttons
    static bool filterClasses = false;
    static bool filterStructs = false;
    static bool filterEnums = false;
    static bool filterInterfaces = false;

    ImGui::Checkbox("Classes", &filterClasses); ImGui::SameLine();
    ImGui::Checkbox("Structs", &filterStructs); ImGui::SameLine();
    ImGui::Checkbox("Enums", &filterEnums); ImGui::SameLine();
    ImGui::Checkbox("Interfaces", &filterInterfaces);

    ImGui::Separator();

    // Type list
    ImGui::BeginChild("TypeListScroll");

    std::string searchStr = searchBuffer_;
    std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), ::tolower);

    for (size_t i = 0; i < assemblyData_.types.size(); i++) {
        const auto& type = assemblyData_.types[i];

        // Apply filters
        if (filterClasses && !type.isClass) continue;
        if (filterStructs && !type.isStruct) continue;
        if (filterEnums && !type.isEnum) continue;
        if (filterInterfaces && !type.isInterface) continue;

        // Apply search filter
        if (!searchStr.empty()) {
            std::string typeName = type.fullName;
            std::transform(typeName.begin(), typeName.end(), typeName.begin(), ::tolower);
            if (typeName.find(searchStr) == std::string::npos) {
                continue;
            }
        }

        // Determine icon based on type
        const char* icon = "?";
        ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

        if (type.isClass) {
            icon = "C";
            color = ImVec4(0.3f, 0.8f, 1.0f, 1.0f);
        } else if (type.isStruct) {
            icon = "S";
            color = ImVec4(0.8f, 0.8f, 0.3f, 1.0f);
        } else if (type.isEnum) {
            icon = "E";
            color = ImVec4(0.8f, 0.3f, 0.8f, 1.0f);
        } else if (type.isInterface) {
            icon = "I";
            color = ImVec4(0.3f, 1.0f, 0.3f, 1.0f);
        }

        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Text("[%s]", icon);
        ImGui::PopStyleColor();

        ImGui::SameLine();

        if (ImGui::Selectable(type.fullName.c_str(), selectedTypeIndex_ == static_cast<int>(i))) {
            selectedTypeIndex_ = static_cast<int>(i);
            currentTab_ = 0; // Reset to first tab
        }

        // Tooltip with additional info
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Name: %s", type.name.c_str());
            ImGui::Text("Namespace: %s", type.namespaceName.c_str());
            ImGui::Text("Base Type: %s", type.baseType.c_str());
            ImGui::Text("Fields: %zu | Methods: %zu | Properties: %zu",
                       type.fields.size(), type.methods.size(), type.properties.size());
            ImGui::EndTooltip();
        }
    }

    ImGui::EndChild();
}

void MainWindow::RenderTypeDetails() {
    if (selectedTypeIndex_ < 0 || selectedTypeIndex_ >= static_cast<int>(assemblyData_.types.size())) {
        ImGui::TextDisabled("Select a type to view details");
        return;
    }

    const auto& type = assemblyData_.types[selectedTypeIndex_];

    // Type header
    ImGui::Text("Type: %s", type.fullName.c_str());
    ImGui::Separator();

    // Type info
    ImGui::Text("Namespace: %s", type.namespaceName.empty() ? "(global)" : type.namespaceName.c_str());
    ImGui::Text("Base Type: %s", type.baseType.empty() ? "None" : type.baseType.c_str());

    ImGui::Text("Kind: ");
    ImGui::SameLine();
    if (type.isClass) { ImGui::TextColored(ImVec4(0.3f, 0.8f, 1.0f, 1.0f), "Class"); }
    if (type.isStruct) { ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.3f, 1.0f), "Struct"); }
    if (type.isEnum) { ImGui::TextColored(ImVec4(0.8f, 0.3f, 0.8f, 1.0f), "Enum"); }
    if (type.isInterface) { ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Interface"); }

    ImGui::Separator();

    // Tabs
    if (ImGui::BeginTabBar("MemberTabs")) {
        if (ImGui::BeginTabItem("Fields")) {
            currentTab_ = 0;
            RenderFieldsTab(type);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Methods")) {
            currentTab_ = 1;
            RenderMethodsTab(type);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Properties")) {
            currentTab_ = 2;
            RenderPropertiesTab(type);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void MainWindow::RenderFieldsTab(const TypeInfo& type) {
    ImGui::Text("Fields (%zu)", type.fields.size());
    ImGui::Separator();

    if (ImGui::BeginTable("FieldsTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 200.0f);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Public", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Static", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("ReadOnly", ImGuiTableColumnFlags_WidthFixed, 70.0f);
        ImGui::TableHeadersRow();

        for (const auto& field : type.fields) {
            if (showPublicOnly_ && !field.isPublic) continue;

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%s", field.name.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 1.0f, 1.0f), "%s", field.fieldType.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", field.isPublic ? "Yes" : "No");

            ImGui::TableNextColumn();
            ImGui::Text("%s", field.isStatic ? "Yes" : "No");

            ImGui::TableNextColumn();
            ImGui::Text("%s", field.isReadOnly ? "Yes" : "No");
        }

        ImGui::EndTable();
    }
}

void MainWindow::RenderMethodsTab(const TypeInfo& type) {
    ImGui::Text("Methods (%zu)", type.methods.size());
    ImGui::Separator();

    if (ImGui::BeginTable("MethodsTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 200.0f);
        ImGui::TableSetupColumn("Signature", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Public", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Static", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableHeadersRow();

        for (const auto& method : type.methods) {
            if (showPublicOnly_ && !method.isPublic) continue;

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%s", method.name.c_str());

            ImGui::TableNextColumn();
            // Build signature
            std::string signature = method.returnType + " " + method.name + "(";
            for (size_t i = 0; i < method.parameters.size(); i++) {
                if (i > 0) signature += ", ";
                signature += method.parameters[i].parameterType + " " + method.parameters[i].name;
            }
            signature += ")";
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.6f, 1.0f), "%s", signature.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", method.isPublic ? "Yes" : "No");

            ImGui::TableNextColumn();
            ImGui::Text("%s", method.isStatic ? "Yes" : "No");
        }

        ImGui::EndTable();
    }
}

void MainWindow::RenderPropertiesTab(const TypeInfo& type) {
    ImGui::Text("Properties (%zu)", type.properties.size());
    ImGui::Separator();

    if (ImGui::BeginTable("PropertiesTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 200.0f);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Get", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Set", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableHeadersRow();

        for (const auto& prop : type.properties) {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%s", prop.name.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "%s", prop.propertyType.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", prop.canRead ? "Yes" : "No");

            ImGui::TableNextColumn();
            ImGui::Text("%s", prop.canWrite ? "Yes" : "No");
        }

        ImGui::EndTable();
    }
}

} // namespace UI
} // namespace UnityReflection
