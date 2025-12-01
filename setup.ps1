# setup.ps1 - Initializes Structure, Configs, and Vendor Dependencies
$ErrorActionPreference = "Stop"
Write-Host "[INIT] STARTING GEOMETRY ENGINE SETUP..." -ForegroundColor Cyan

$rootDir = $PWD
$vendorDir = "$rootDir\Vendor"

# ==========================================
# 1. CREATE PROJECT DIRECTORIES
# ==========================================
Write-Host "[DIR] Creating Directory Structure..." -ForegroundColor Yellow
$dirs = @("Shared", "Backend", "Bridge", "Frontend", "Tests", "Vendor")
foreach ($d in $dirs) {
    if (-not (Test-Path $d)) { New-Item -ItemType Directory -Force -Path $d | Out-Null }
}

# ==========================================
# 2. GENERATE INTERNAL CMAKE FILES
# ==========================================
Write-Host "[CMAKE] Generating CMakeLists.txt..." -ForegroundColor Yellow

# Root CMakeLists.txt
@"
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
    "`${IMGUI_DIR}/imgui.cpp"
    "`${IMGUI_DIR}/imgui_draw.cpp"
    "`${IMGUI_DIR}/imgui_tables.cpp"
    "`${IMGUI_DIR}/imgui_widgets.cpp"
    "`${IMGUI_DIR}/imgui_demo.cpp"
    "`${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "`${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)

add_library(ImGuiLib STATIC `${IMGUI_SOURCES})

target_include_directories(ImGuiLib PUBLIC "`${IMGUI_DIR}" "`${IMGUI_DIR}/backends")
target_link_libraries(ImGuiLib PUBLIC glfw)

# --- ENGINE MODULES ---
add_subdirectory(Shared)
add_subdirectory(Backend)
add_subdirectory(Bridge)
add_subdirectory(Frontend)
"@ | Out-File "CMakeLists.txt" -Encoding UTF8

# Shared/CMakeLists.txt
@"
project(Shared)
add_library(Shared INTERFACE)
target_include_directories(Shared INTERFACE `${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Shared INTERFACE glm spdlog::spdlog nlohmann_json::nlohmann_json)
target_compile_features(Shared INTERFACE cxx_std_23)
"@ | Out-File "Shared/CMakeLists.txt" -Encoding UTF8

# Backend/CMakeLists.txt
@"
project(Backend)
file(GLOB_RECURSE SOURCES "*.cpp" "*.h" "*.hpp")
add_library(Backend SHARED `${SOURCES})

target_include_directories(Backend PUBLIC 
    `${CMAKE_CURRENT_SOURCE_DIR}
    `${CMAKE_SOURCE_DIR}/Vendor/entt/src
    `${CMAKE_SOURCE_DIR}/Vendor/glm
    `${CMAKE_SOURCE_DIR}/Vendor/json/include
    `${CMAKE_SOURCE_DIR}/Vendor/spdlog/include
    `${CMAKE_SOURCE_DIR}/Vendor/bgfx-cmake/bgfx/include
    `${CMAKE_SOURCE_DIR}/Vendor/bgfx-cmake/bx/include
    `${CMAKE_SOURCE_DIR}/Vendor/bgfx-cmake/bimg/include
)

target_link_libraries(Backend PUBLIC Shared bgfx bimg bx glfw spdlog::spdlog)

if(WIN32)
    target_compile_definitions(Backend PRIVATE BACKEND_EXPORTS)
endif()
target_compile_features(Backend PUBLIC cxx_std_23)
"@ | Out-File "Backend/CMakeLists.txt" -Encoding UTF8

# Bridge/CMakeLists.txt
@"
project(Bridge)
file(GLOB_RECURSE SOURCES "*.cpp" "*.h" "*.hpp")
add_library(Bridge SHARED `${SOURCES})
target_include_directories(Bridge PUBLIC `${CMAKE_CURRENT_SOURCE_DIR} `${CMAKE_SOURCE_DIR}/Vendor/httplib)
target_link_libraries(Bridge PUBLIC Shared Backend)
if(WIN32)
    target_link_libraries(Bridge PUBLIC ws2_32 crypt32)
    target_compile_definitions(Bridge PRIVATE BRIDGE_EXPORTS)
endif()
target_compile_features(Bridge PUBLIC cxx_std_23)
"@ | Out-File "Bridge/CMakeLists.txt" -Encoding UTF8

# Frontend/CMakeLists.txt
@"
project(Frontend)
file(GLOB_RECURSE SOURCES "*.cpp" "*.h" "*.hpp")
add_executable(Editor `${SOURCES})

find_package(OpenGL REQUIRED)

target_include_directories(Editor PUBLIC `${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Editor PRIVATE 
    Backend 
    Bridge 
    Shared 
    ImGuiLib 
    glfw 
    OpenGL::GL
)

if(WIN32)
    add_custom_command(TARGET Editor POST_BUILD
        COMMAND `${CMAKE_COMMAND} -E copy_if_different `$<TARGET_FILE:Backend> `$<TARGET_FILE_DIR:Editor>
        COMMAND `${CMAKE_COMMAND} -E copy_if_different `$<TARGET_FILE:Bridge> `$<TARGET_FILE_DIR:Editor>
        COMMAND `${CMAKE_COMMAND} -E copy_if_different `$<TARGET_FILE:bgfx> `$<TARGET_FILE_DIR:Editor>
    )
endif()
target_compile_features(Editor PRIVATE cxx_std_23)
"@ | Out-File "Frontend/CMakeLists.txt" -Encoding UTF8

# ==========================================
# 3. GENERATE DUMMY SOURCE FILES
# ==========================================
if (-not (Test-Path "Backend/Engine.cpp")) {
@"
#include <iostream>
#if defined(_WIN32)
    #define BACKEND_API __declspec(dllexport)
#else
    #define BACKEND_API
#endif
namespace Backend { BACKEND_API void Init() { std::cout << "Engine Init"; } }
"@ | Out-File "Backend/Engine.cpp" -Encoding UTF8
}

if (-not (Test-Path "Bridge/Bridge.cpp")) {
@"
#include <iostream>
#if defined(_WIN32)
    #define BRIDGE_API __declspec(dllexport)
#else
    #define BRIDGE_API
#endif
namespace Bridge { BRIDGE_API void Init() { std::cout << "Bridge Init"; } }
"@ | Out-File "Bridge/Bridge.cpp" -Encoding UTF8
}

if (-not (Test-Path "Frontend/Main.cpp")) {
@"
#include <iostream>
int main() { std::cout << "Editor Start"; return 0; }
"@ | Out-File "Frontend/Main.cpp" -Encoding UTF8
}

# ==========================================
# 4. DOWNLOAD VENDOR LIBRARIES
# ==========================================
Write-Host "[VENDOR] Synchronizing Libraries..." -ForegroundColor Yellow

$libs = @(
    @{ Name="imgui"; Url="https://github.com/ocornut/imgui.git"; Branch="docking" },
    @{ Name="glfw";  Url="https://github.com/glfw/glfw.git";     Branch="master" },
    @{ Name="glm";   Url="https://github.com/g-truc/glm.git";    Branch="master" },
    @{ Name="entt";  Url="https://github.com/skypjack/entt.git"; Branch="v3.13.2" },
    @{ Name="spdlog"; Url="https://github.com/gabime/spdlog.git"; Branch="v1.12.0" },
    @{ Name="json";   Url="https://github.com/nlohmann/json.git"; Branch="v3.11.3" },
    @{ Name="bgfx-cmake"; Url="https://github.com/bkaradzic/bgfx.cmake.git"; Branch="master" },
    @{ Name="httplib"; Url="https://github.com/yhirose/cpp-httplib.git"; Branch="master" }
)

foreach ($lib in $libs) {
    $path = "Vendor/$($lib.Name)"
    
    # 1. Clean up broken/empty folders
    if ((Test-Path $path) -and (-not (Test-Path "$path/.git"))) {
        Write-Host "   [CLEAN] Removing broken folder: $($lib.Name)" -ForegroundColor Red
        Remove-Item -Recurse -Force $path
    }

    # 2. Clone if missing
    if (-not (Test-Path "$path/.git")) {
        Write-Host "   [CLONE] Cloning $($lib.Name)..." -ForegroundColor Cyan
        git clone --depth 1 --branch $lib.Branch $lib.Url $path 2>$null
    } else {
        # 3. Branch Check (Self-Healing)
        Push-Location $path
        try {
            $currentBranch = git branch --show-current
            if ($currentBranch -ne $lib.Branch) {
                Write-Host "   [SWITCH] Switching $($lib.Name) to $($lib.Branch)..." -ForegroundColor Yellow
                git fetch origin $lib.Branch 2>$null
                git checkout -f $lib.Branch 2>$null
                git reset --hard "origin/$($lib.Branch)" 2>$null
            } else {
                Write-Host "   [SKIP] $($lib.Name) ready." -ForegroundColor Gray
            }
        }
        catch {
             Write-Host "   [WARN] Could not update $($lib.Name)" -ForegroundColor Yellow
        }
        finally {
            Pop-Location
        }
    }
}

# Sync BGFX Submodules
if (Test-Path "Vendor/bgfx-cmake") {
    Write-Host "   [SYNC] Updating BGFX submodules..." -ForegroundColor Cyan
    Push-Location "Vendor/bgfx-cmake"
    git submodule update --init --recursive 2>$null
    Pop-Location
}

Write-Host "[DONE] SETUP COMPLETE! Ready to run '3_build.ps1'" -ForegroundColor Green