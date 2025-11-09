#pragma once

#include "../reflection_data.h"
#include <string>
#include <vector>

namespace UnityReflection {
namespace UI {

class MainWindow {
public:
    MainWindow();
    ~MainWindow();

    void SetAssemblyData(const AssemblyData& data);
    void Render();

private:
    void RenderConnectionStatus();
    void RenderTypeList();
    void RenderTypeDetails();
    void RenderFieldsTab(const TypeInfo& type);
    void RenderMethodsTab(const TypeInfo& type);
    void RenderPropertiesTab(const TypeInfo& type);

    AssemblyData assemblyData_;
    int selectedTypeIndex_ = -1;
    char searchBuffer_[256] = {0};
    bool showPublicOnly_ = false;
    bool showInheritedMembers_ = false;

    // Tab selection
    int currentTab_ = 0; // 0=Fields, 1=Methods, 2=Properties

    // Stats
    int totalClasses_ = 0;
    int totalStructs_ = 0;
    int totalEnums_ = 0;
    int totalInterfaces_ = 0;
};

} // namespace UI
} // namespace UnityReflection
