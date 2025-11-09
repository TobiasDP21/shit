cmake_minimum_required(VERSION 3.15)
project(UnityReflectionViewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectories
add_subdirectory(external/imgui)

# Source files
set(SOURCES
    src/main.cpp
    src/ipc_client.cpp
    src/reflection_data.cpp
    src/ui/main_window.cpp
)

set(HEADERS
    src/ipc_client.h
    src/reflection_data.h
    src/ui/main_window.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
elseif(UNIX AND NOT APPLE)
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL dl pthread)
elseif(APPLE)
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE src)
