#include "WindowSetup.h"
#include "UILayouts.h"

// Backend API
namespace Backend { extern void Init(); }

int main(int, char**) {
    // 1. Configure
    WindowSetup::WindowConfig config;
    config.title = "Geometry Engine";
    config.width = 1920;
    config.height = 1080;
    
    // FIX 1: 'on_init' -> 'on_post_init' (Callback signature changed)
    config.on_post_init = [](GLFWwindow* window) {
        std::cout << "[INFO] Initializing Engine Backend..." << std::endl;
        
        // Initialize your engine backend here
        Backend::Init(); 
        
        // Optional: Nice touch for the main editor window
        WindowSetup::CenterWindow();
    };

    // FIX 2: 'Init' -> 'Initialize'
    // This sets up GLFW, ImGui, VSync, and Fonts automatically
    if (!WindowSetup::Initialize(config)) return 1;

    // 2. Main Loop
    // FIX 3: Use WindowSetup::ShouldClose() instead of manual glfw calls
    while (!WindowSetup::ShouldClose()) {
        
        // FIX 4: Centralized frame start (Handles PollEvents + NewFrame)
        WindowSetup::BeginFrame();

        // FIX 5: Use wrapper for DockSpace 
        // This replaces 'ImGui::DockSpaceOverViewport' and handles flags/IDs
        WindowSetup::BeginDockspace("EditorDockSpace");

        // --- RENDER EDITOR UI ---
        UILab::Render(); 
        // ------------------------

        WindowSetup::EndDockspace();
        WindowSetup::EndFrame();
        
        // FIX 6: WindowSetup::Render() handles:
        // - glClearColor
        // - glViewport
        // - ImGui_ImplOpenGL3_RenderDrawData
        // - UpdatePlatformWindows (Multi-Viewports)
        // - glfwSwapBuffers
        WindowSetup::Render();
    }

    // FIX 7: Shutdown takes no arguments now
    WindowSetup::Shutdown();
    return 0;
}