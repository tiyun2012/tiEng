project(Backend)
file(GLOB_RECURSE SOURCES "*.cpp" "*.h" "*.hpp")
add_library(Backend SHARED ${SOURCES})

target_include_directories(Backend PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Vendor/entt/src
    ${CMAKE_SOURCE_DIR}/Vendor/glm
    ${CMAKE_SOURCE_DIR}/Vendor/json/include
    ${CMAKE_SOURCE_DIR}/Vendor/spdlog/include
    ${CMAKE_SOURCE_DIR}/Vendor/bgfx-cmake/bgfx/include
    ${CMAKE_SOURCE_DIR}/Vendor/bgfx-cmake/bx/include
    ${CMAKE_SOURCE_DIR}/Vendor/bgfx-cmake/bimg/include
)

target_link_libraries(Backend PUBLIC Shared bgfx bimg bx glfw spdlog::spdlog)

if(WIN32)
    target_compile_definitions(Backend PRIVATE BACKEND_EXPORTS)
endif()
target_compile_features(Backend PUBLIC cxx_std_23)
