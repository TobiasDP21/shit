cmake_minimum_required(VERSION 3.15)
project(imgui)

# ImGui core files (you'll need to download ImGui from https://github.com/ocornut/imgui)
set(IMGUI_SOURCES
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
)

set(IMGUI_HEADERS
    imgui.h
    imconfig.h
    imgui_internal.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
)

# Backend files - using GLFW + OpenGL3
set(BACKEND_SOURCES
    backends/imgui_impl_glfw.cpp
    backends/imgui_impl_opengl3.cpp
)

set(BACKEND_HEADERS
    backends/imgui_impl_glfw.h
    backends/imgui_impl_opengl3.h
)

# Find GLFW
find_package(glfw3 REQUIRED)

# Create library
add_library(imgui STATIC
    ${IMGUI_SOURCES}
    ${IMGUI_HEADERS}
    ${BACKEND_SOURCES}
    ${BACKEND_HEADERS}
)

# Include directories
target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/backends
)

# Link GLFW
target_link_libraries(imgui PUBLIC glfw)

# OpenGL
if(WIN32)
    target_link_libraries(imgui PUBLIC opengl32)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(imgui PUBLIC OpenGL::GL)
endif()

# Definitions
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
