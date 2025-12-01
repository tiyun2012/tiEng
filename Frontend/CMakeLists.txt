project(Frontend)

# --- 0. GATHER RESOURCES ---
# Grab all UI headers so they show up in the IDE for both projects
file(GLOB_RECURSE UI_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/WindowSetup.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/UI/*.h"
)

find_package(OpenGL REQUIRED)

# --- 1. TARGET: EDITOR (Full Engine) ---
# Grab all source files
file(GLOB_RECURSE EDITOR_SOURCES "*.cpp" "*.h" "*.hpp")
# EXCLUDE SandboxMain.cpp from the Editor executable
list(FILTER EDITOR_SOURCES EXCLUDE REGEX ".*SandboxMain\\.cpp$")

add_executable(Editor ${EDITOR_SOURCES})

target_include_directories(Editor PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/UI
)

target_link_libraries(Editor PRIVATE Backend Bridge Shared ImGuiLib glfw OpenGL::GL)
target_compile_features(Editor PRIVATE cxx_std_23)

# --- 2. TARGET: SANDBOX (Rapid UI Iteration) ---
# We add UI_HEADERS here so they appear in the Solution Explorer
add_executable(UISandbox 
    SandboxMain.cpp 
    ${UI_HEADERS}
)

target_include_directories(UISandbox PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/UI
)

# Sandbox does NOT link Backend, making it fast and lightweight
target_link_libraries(UISandbox PRIVATE ImGuiLib glfw OpenGL::GL)
target_compile_features(UISandbox PRIVATE cxx_std_23)

# --- 3. POST-BUILD: ASSET SYNC ---
# Define a macro to copy assets for any target
function(copy_assets TARGET_NAME)
    if(EXISTS "${CMAKE_SOURCE_DIR}/Assets")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/Assets"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Assets" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Assets"
            COMMENT "Copying Assets to ${TARGET_NAME} output directory..."
        )
    endif()
endfunction()

copy_assets(Editor)
copy_assets(UISandbox)

# --- 4. CONFIGURATION ---
# Standardize output directories
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

# Enable high warning levels
if(MSVC)
    target_compile_options(Editor PRIVATE /W4)
    target_compile_options(UISandbox PRIVATE /W4)
endif()