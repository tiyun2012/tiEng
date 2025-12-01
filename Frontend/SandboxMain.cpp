#include <iostream>
#include "WindowSetup.h"
#include "UILayouts.h"


int main(int, char**) {
    WindowSetup::WindowConfig config;
    config.title = "UI SANDBOX";
    config.on_post_init = [](GLFWwindow*) { std::cout << "Sandbox Ready.\n"; };

    // 1. Initialize
    if (!WindowSetup::Initialize(config)) return 1;

    // 2. Loop
    while (!WindowSetup::ShouldClose()) {
        
        // This helper handles ImGui::NewFrame() AND the Polling
        WindowSetup::BeginFrame();

        // FIX: Don't call ImGui::DockSpaceOverViewport manually.
        // Use the wrapper in WindowSetup.h which handles the flags and ID stack.
        WindowSetup::BeginDockspace("SandboxDockSpace");

        // --- RENDER YOUR UI PANELS HERE ---
        UILab::Render(); 
        // ----------------------------------

        WindowSetup::EndDockspace();
        WindowSetup::EndFrame();

        // This helper handles Clear, Viewports, and SwapBuffers
        WindowSetup::Render();
    }

    WindowSetup::Shutdown();
    return 0;
}