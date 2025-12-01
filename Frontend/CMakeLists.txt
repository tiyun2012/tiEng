project(Frontend)
file(GLOB_RECURSE SOURCES "*.cpp" "*.h" "*.hpp")
add_executable(Editor ${SOURCES})

find_package(OpenGL REQUIRED)

target_include_directories(Editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

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
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Backend> $<TARGET_FILE_DIR:Editor>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Bridge> $<TARGET_FILE_DIR:Editor>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:bgfx> $<TARGET_FILE_DIR:Editor>
    )
endif()
target_compile_features(Editor PRIVATE cxx_std_23)
