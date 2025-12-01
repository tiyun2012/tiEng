/*
 * SAFE MODE MAIN.CPP
 * Goal: Get the window open without crashing.
 * Removed: Custom Fonts, Icons, Strict OpenGL Versioning
 */

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>

// Backend API (Forward Declaration)
namespace Backend { extern void Init(); }

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char**) {
    // 1. Setup GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        std::cerr << "[ERROR] Failed to init GLFW\n";
        return 1;
    }

    // --- SAFETY FIX: Comment out strict version hints ---
    // Letting GLFW pick the default allows the internal loader to work more reliably.
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Auto-detect GLSL version
    const char* glsl_version = "#version 130"; 

    // 2. Create Window
    std::cout << "[INFO] Creating Window...\n";
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Geometry Engine (Safe Mode)", nullptr, nullptr);
    if (window == nullptr) return 1;
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // 3. Setup ImGui
    std::cout << "[INFO] Initializing ImGui...\n";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; 
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 

    // Setup Style
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // --- SAFETY FIX: Remove Custom Font Loading for now ---
    // We will add fonts back AFTER we confirm the window opens.
    // io.Fonts->AddFontFromFileTTF(...) <--- This was likely crashing because files don't exist

    // 4. Init Backends
    std::cout << "[INFO] Initializing Backends...\n";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    
    if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
        std::cerr << "[ERROR] Failed to initialize OpenGL3 Backend!\n";
        return 1;
    }

    // 5. Init Engine
    Backend::Init();

    // 6. Main Loop
    std::cout << "[INFO] Starting Loop...\n";
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // DockSpace
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        // UI
        ImGui::ShowDemoWindow();
        
        ImGui::Begin("Debug Info");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}