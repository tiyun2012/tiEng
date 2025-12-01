cmake_minimum_required(VERSION 3.25)
project(GeometryEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- VENDOR CONFIGURATION ---
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(ENTT_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BGFX_BUILD_TOOLS OFF CACHE BOOL "" FORCE)

# Add Standard Vendor Directories
add_subdirectory(Vendor/glfw)
add_subdirectory(Vendor/glm)
add_subdirectory(Vendor/spdlog)
add_subdirectory(Vendor/json)
add_subdirectory(Vendor/bgfx-cmake)

# --- IMGUI CONFIGURATION (Explicit Selection) ---
set(IMGUI_DIR "Vendor/imgui")

# FIX: Manually list ONLY the files we need. Do NOT use *.cpp in backends!
set(IMGUI_SOURCES
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)

add_library(ImGuiLib STATIC ${IMGUI_SOURCES})

target_include_directories(ImGuiLib PUBLIC "${IMGUI_DIR}" "${IMGUI_DIR}/backends")
target_link_libraries(ImGuiLib PUBLIC glfw)

# --- ENGINE MODULES ---
add_subdirectory(Shared)
add_subdirectory(Backend)
add_subdirectory(Bridge)
add_subdirectory(Frontend)
