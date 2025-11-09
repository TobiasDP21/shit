#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

#include "ipc_client.h"
#include "reflection_data.h"
#include "ui/main_window.h"

static void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

int main(int argc, char** argv) {
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return 1;
    }

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Unity Reflection Viewer", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Create main window
    auto mainWindow = std::make_unique<UnityReflection::UI::MainWindow>();

    // Create IPC client
    auto ipcClient = std::make_unique<UnityReflection::IPCClient>();

    // Set up callbacks
    ipcClient->SetDataCallback([&mainWindow](const std::string& data) {
        std::cout << "Received data: " << data.length() << " bytes" << std::endl;

        UnityReflection::AssemblyData assemblyData;
        if (UnityReflection::ParseAssemblyData(data, assemblyData)) {
            std::cout << "Successfully parsed assembly: " << assemblyData.assemblyName << std::endl;
            std::cout << "Total types: " << assemblyData.types.size() << std::endl;
            mainWindow->SetAssemblyData(assemblyData);
        } else {
            std::cerr << "Failed to parse assembly data" << std::endl;
        }
    });

    ipcClient->SetErrorCallback([](const std::string& error) {
        std::cerr << "IPC Error: " << error << std::endl;
    });

    // Start listening for connections
    std::cout << "Starting IPC listener..." << std::endl;
    std::cout << "Waiting for Unity to connect..." << std::endl;
    ipcClient->StartListening();

    // Main loop
    ImVec4 clear_color = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render main window
        mainWindow->Render();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                     clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ipcClient->StopListening();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
