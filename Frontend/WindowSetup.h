#pragma once

// Version: 2.1.0
// Purpose: Unified window and ImGui setup for Geometry Engine
// Features: Docking (Fixed), Viewports, Themes, Error Handling

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <functional>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>
#include <chrono>

// Include your icon definitions
// Ensure this path matches your file structure relative to WindowSetup.h
#include "UI/Core/IconsFontAwesome6.h"

namespace WindowSetup {

    // ============================================================================
    // FORWARD DECLARATIONS
    // ============================================================================
    struct WindowConfig;
    struct PerformanceMetrics;
    class ScopedTimer;

    // ============================================================================
    // ENUMS & CONSTANTS
    // ============================================================================
    enum class WindowMode {
        Windowed,
        BorderlessFullscreen,
        Fullscreen
    };

    enum class VSyncMode {
        Disabled = 0,
        Enabled = 1,
        Adaptive = -1  // GLFW: -1 for adaptive vsync
    };

    enum class ThemePreset {
        ClassicDark,
        ClassicLight,
        Corporate,
        Dracula,
        MaterialDark,
        Cherry
    };

    // ============================================================================
    // CONFIGURATION STRUCTURES
    // ============================================================================

    struct WindowConfig {
        // Basic window settings
        std::string title = "Geometry Engine";
        int width = 1600;
        int height = 900;
        WindowMode mode = WindowMode::Windowed;
        VSyncMode vsync = VSyncMode::Enabled;
        
        // OpenGL settings
        int gl_major = 3;
        int gl_minor = 3;
        bool core_profile = true;
        bool forward_compat = true;
        bool double_buffer = true;
        int samples = 4;  // MSAA samples
        
        // ImGui settings
        bool docking_enabled = true;
        bool viewports_enabled = true;
        bool keyboard_navigation = true;
        bool gamepad_navigation = false;
        const char* glsl_version = "#version 130";
        
        // Font settings
        struct {
            float size = 16.0f;
            std::string regular_path = "Assets/Fonts/Roboto-Regular.ttf";
            std::string bold_path = "Assets/Fonts/Roboto-Bold.ttf";
            std::string mono_path = "Assets/Fonts/RobotoMono-Regular.ttf";
            bool load_font_awesome = true;
            std::string font_awesome_path = "Assets/Fonts/fa-solid-900.ttf";
            float font_awesome_size_multiplier = 0.8f;
        } fonts;
        
        // Callbacks
        std::function<void(GLFWwindow*)> on_pre_init = nullptr;
        std::function<void(GLFWwindow*)> on_post_init = nullptr;
        std::function<void()> on_frame_start = nullptr;
        std::function<void()> on_frame_end = nullptr;
        std::function<void()> on_shutdown = nullptr;
        
        // Debug settings
        bool log_initialization = true;
        bool assert_on_error = true;
        bool enable_debug_output = true;
    };

    struct PerformanceMetrics {
        double frame_time_ms = 0.0;
        double fps = 0.0;
        double input_latency_ms = 0.0;
        double render_time_ms = 0.0;
        double update_time_ms = 0.0;
        size_t draw_calls = 0;
        size_t vertices = 0;
        size_t indices = 0;
        
        void Reset() {
            frame_time_ms = 0.0;
            fps = 0.0;
            input_latency_ms = 0.0;
            render_time_ms = 0.0;
            update_time_ms = 0.0;
            draw_calls = 0;
            vertices = 0;
            indices = 0;
        }
    };

    // ============================================================================
    // UTILITY CLASSES
    // ============================================================================

    class ScopedTimer {
    public:
        ScopedTimer(const std::string& name) : m_name(name) {
            m_start = std::chrono::high_resolution_clock::now();
        }
        
        ~ScopedTimer() {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - m_start);
            std::cout << "[TIMER] " << m_name << ": " << duration.count() << "us" << std::endl;
        }
        
    private:
        std::string m_name;
        std::chrono::high_resolution_clock::time_point m_start;
    };

    // ============================================================================
    // STATIC DATA & INITIALIZATION
    // ============================================================================

    // Global state (internal use only)
    namespace Internal {
        static GLFWwindow* s_window = nullptr;
        static WindowConfig s_config;
        static PerformanceMetrics s_metrics;
        static bool s_initialized = false;
        static bool s_should_close = false;
        static std::chrono::high_resolution_clock::time_point s_frame_start;
        
        // Font cache
        static ImFont* s_main_font = nullptr;
        static ImFont* s_bold_font = nullptr;
        static ImFont* s_mono_font = nullptr;
        static ImFont* s_icon_font = nullptr;
        
        // Error callback
        static void glfw_error_callback(int error, const char* description) {
            std::cerr << "[GLFW Error " << error << "]: " << description << std::endl;
            if (s_config.assert_on_error) {
                IM_ASSERT(false && "GLFW error occurred");
            }
        }
        
        // Window close callback
        static void window_close_callback(GLFWwindow* window) {
            (void)window; // Suppress unused parameter warning
            s_should_close = true;
        }
                
        // Framebuffer resize callback
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
            (void)window; // Suppress unused parameter warning
            if (width > 0 && height > 0) {
                s_config.width = width;
                s_config.height = height;
                std::cout << "[WINDOW] Resized to " << width << "x" << height << std::endl;
            }
        }
    }

    // ============================================================================
    // FONT MANAGEMENT
    // ============================================================================

    inline bool LoadFonts(const WindowConfig& config) {
        IMGUI_CHECKVERSION();
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();
        
        bool success = true;
        
        // Helper function to load font with fallback
        auto load_font_with_fallback = [&](const std::string& path, float size, 
                                          const ImWchar* ranges = nullptr, 
                                          const ImFontConfig* config = nullptr) -> ImFont* {
            if (std::filesystem::exists(path)) {
                try {
                    return io.Fonts->AddFontFromFileTTF(path.c_str(), size, config, ranges);
                } catch (const std::exception& e) {
                    std::cerr << "[FONT] Failed to load " << path << ": " << e.what() << std::endl;
                }
            }
            
            // Try alternative paths
            std::vector<std::string> alternative_paths = {
                "./" + path,
                "../" + path,
                "../../" + path,
                path.substr(path.find_last_of('/') + 1)
            };
            
            for (const auto& alt_path : alternative_paths) {
                if (std::filesystem::exists(alt_path)) {
                    std::cout << "[FONT] Found alternative: " << alt_path << std::endl;
                    try {
                        return io.Fonts->AddFontFromFileTTF(alt_path.c_str(), size, config, ranges);
                    } catch (...) {
                        continue;
                    }
                }
            }
            
            std::cerr << "[FONT] Using default for: " << path << std::endl;
            return nullptr;
        };
        
        // Load main font
        Internal::s_main_font = load_font_with_fallback(config.fonts.regular_path, config.fonts.size);
        if (!Internal::s_main_font) {
            Internal::s_main_font = io.Fonts->AddFontDefault();
            success = false;
        }
        
        // Load bold font (optional)
        if (!config.fonts.bold_path.empty()) {
            Internal::s_bold_font = load_font_with_fallback(config.fonts.bold_path, config.fonts.size * 1.1f);
        }
        
        // Load monospace font (optional)
        if (!config.fonts.mono_path.empty()) {
            static const ImWchar mono_ranges[] = { 0x0020, 0x00FF, 0 }; // Basic Latin
            Internal::s_mono_font = load_font_with_fallback(config.fonts.mono_path, config.fonts.size * 0.9f, mono_ranges);
        }
        
        // Load Font Awesome icons (merged with main font)
        if (config.fonts.load_font_awesome && !config.fonts.font_awesome_path.empty()) {
            static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
            ImFontConfig icons_config;
            icons_config.MergeMode = true;
            icons_config.GlyphMinAdvanceX = config.fonts.size;
            icons_config.PixelSnapH = true;
            
            Internal::s_icon_font = load_font_with_fallback(
                config.fonts.font_awesome_path,
                config.fonts.size * config.fonts.font_awesome_size_multiplier,
                icon_ranges,
                &icons_config
            );
            
            if (Internal::s_icon_font) {
                std::cout << "[FONT] Font Awesome icons loaded successfully" << std::endl;
            } else {
                std::cerr << "[FONT] Failed to load Font Awesome icons" << std::endl;
                success = false;
            }
        }
        
        // io.Fonts->Build(); // Handled by ImGui_ImplOpenGL3_NewFrame
        return success;
    }

    // ============================================================================
    // THEME MANAGEMENT
    // ============================================================================

    inline void ApplyTheme(ThemePreset theme) {
        ImGuiStyle& style = ImGui::GetStyle();
        
        switch (theme) {
            case ThemePreset::ClassicDark:
                ImGui::StyleColorsDark();
                break;
                
            case ThemePreset::ClassicLight:
                ImGui::StyleColorsLight();
                break;
                
            case ThemePreset::Corporate:
                ImGui::StyleColorsDark();
                style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.08f, 0.12f, 0.94f);
                style.Colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.12f, 0.15f, 1.00f);
                style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.10f, 0.14f, 0.94f);
                style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
                style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
                style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.30f, 0.35f, 1.00f);
                style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.10f, 0.14f, 1.00f);
                style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.12f, 0.16f, 1.00f);
                break;
                
            case ThemePreset::Dracula: {
                // Simplified Dracula-inspired setup
                ImGui::StyleColorsDark();
                auto& colors = style.Colors;
                colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
                colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
                colors[ImGuiCol_CheckMark] = ImVec4(0.58f, 0.41f, 0.93f, 1.00f);
                colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
                colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 0.90f, 1.00f);
                colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
                break;
            }
                
            case ThemePreset::MaterialDark: {
                ImGui::StyleColorsDark();
                style.WindowRounding = 8.0f;
                style.FrameRounding = 4.0f;
                style.WindowPadding = ImVec2(8, 8);
                break;
            }
                
            case ThemePreset::Cherry: {
                ImGui::StyleColorsDark();
                style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 0.96f, 0.98f, 1.00f);
                style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 0.94f);
                style.Colors[ImGuiCol_Button] = ImVec4(0.44f, 0.16f, 0.42f, 0.40f);
                style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.94f, 0.26f, 0.86f, 1.00f);
                style.Colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.26f, 0.86f, 1.00f);
                break;
            }
        }
    }

    // ============================================================================
    // CORE WINDOW FUNCTIONS
    // ============================================================================

    inline GLFWwindow* Initialize(const WindowConfig& config = WindowConfig()) {
        if (Internal::s_initialized) {
            std::cerr << "[ERROR] Window already initialized!" << std::endl;
            return Internal::s_window;
        }
        
        Internal::s_config = config;
        
        // Start timer for initialization
        auto init_start = std::chrono::high_resolution_clock::now();
        
        // Set error callback
        glfwSetErrorCallback(Internal::glfw_error_callback);
        
        // Initialize GLFW
        if (!glfwInit()) {
            std::cerr << "[FATAL] Failed to initialize GLFW" << std::endl;
            return nullptr;
        }
        
        // Pre-init callback
        if (config.on_pre_init) {
            config.on_pre_init(nullptr);
        }
        
        // Set window hints
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.gl_major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.gl_minor);
        if (config.core_profile) {
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }
        if (config.forward_compat) {
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        }
        if (config.double_buffer) {
            glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
        }
        if (config.samples > 1) {
            glfwWindowHint(GLFW_SAMPLES, config.samples);
        }
        
        // Create window based on mode
        GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* video_mode = glfwGetVideoMode(primary_monitor);
        
        switch (config.mode) {
            case WindowMode::Fullscreen:
                Internal::s_window = glfwCreateWindow(
                    video_mode->width, video_mode->height,
                    config.title.c_str(), primary_monitor, nullptr);
                break;
                
            case WindowMode::BorderlessFullscreen:
                glfwWindowHint(GLFW_DECORATED, GL_FALSE);
                Internal::s_window = glfwCreateWindow(
                    video_mode->width, video_mode->height,
                    config.title.c_str(), nullptr, nullptr);
                break;
                
            case WindowMode::Windowed:
            default:
                Internal::s_window = glfwCreateWindow(
                    config.width, config.height,
                    config.title.c_str(), nullptr, nullptr);
                break;
        }
        
        if (!Internal::s_window) {
            std::cerr << "[FATAL] Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return nullptr;
        }
        
        // Make context current
        glfwMakeContextCurrent(Internal::s_window);
        
        // Set vsync
        glfwSwapInterval(static_cast<int>(config.vsync));
        
        // Set callbacks
        glfwSetWindowCloseCallback(Internal::s_window, Internal::window_close_callback);
        glfwSetFramebufferSizeCallback(Internal::s_window, Internal::framebuffer_size_callback);
        
        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        
        // Set ImGui configuration flags
        if (config.docking_enabled) {
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        }
        if (config.viewports_enabled) {
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
            ImGui::GetStyle().WindowRounding = 0.0f;
            ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
        if (config.keyboard_navigation) {
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        }
        if (config.gamepad_navigation) {
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        }
        
        // Load fonts
        if (!LoadFonts(config)) {
            std::cerr << "[WARNING] Some fonts failed to load, using defaults" << std::endl;
        }
        
        // Initialize ImGui platform/renderer backends
        if (!ImGui_ImplGlfw_InitForOpenGL(Internal::s_window, true)) {
            std::cerr << "[FATAL] Failed to initialize ImGui GLFW backend" << std::endl;
            return nullptr;
        }
        
        if (!ImGui_ImplOpenGL3_Init(config.glsl_version)) {
            std::cerr << "[FATAL] Failed to initialize ImGui OpenGL3 backend" << std::endl;
            return nullptr;
        }
        
        // Apply default theme
        ApplyTheme(ThemePreset::ClassicDark);
        
        // Set initialized flag
        Internal::s_initialized = true;
        Internal::s_should_close = false;
        
        // Post-init callback
        if (config.on_post_init) {
            config.on_post_init(Internal::s_window);
        }
        
        // Calculate initialization time
        auto init_end = std::chrono::high_resolution_clock::now();
        auto init_duration = std::chrono::duration_cast<std::chrono::milliseconds>(init_end - init_start);
        
        if (config.log_initialization) {
            std::cout << "[INIT] Window initialized in " << init_duration.count() << "ms" << std::endl;
            std::cout << "[INIT] OpenGL " << config.gl_major << "." << config.gl_minor 
                      << " | " << config.width << "x" << config.height 
                      << " | " << config.title << std::endl;
        }
        
        return Internal::s_window;
    }

    inline void BeginFrame() {
        if (!Internal::s_initialized) return;
        
        Internal::s_frame_start = std::chrono::high_resolution_clock::now();
        
        // Poll events
        glfwPollEvents();
        
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // Frame start callback
        if (Internal::s_config.on_frame_start) {
            Internal::s_config.on_frame_start();
        }
    }

    inline void EndFrame() {
        if (!Internal::s_initialized) return;
        
        // Frame end callback
        if (Internal::s_config.on_frame_end) {
            Internal::s_config.on_frame_end();
        }
        
        // Update performance metrics
        auto frame_end = std::chrono::high_resolution_clock::now();
        auto frame_duration = std::chrono::duration_cast<std::chrono::microseconds>(
            frame_end - Internal::s_frame_start);
        
        Internal::s_metrics.frame_time_ms = frame_duration.count() / 1000.0;
        Internal::s_metrics.fps = 1000000.0 / frame_duration.count();
        
        // Update draw statistics
        ImDrawData* draw_data = ImGui::GetDrawData();
        if (draw_data) {
            Internal::s_metrics.draw_calls = draw_data->CmdListsCount;
            Internal::s_metrics.vertices = 0;
            Internal::s_metrics.indices = 0;
            for (int i = 0; i < draw_data->CmdListsCount; i++) {
                const ImDrawList* cmd_list = draw_data->CmdLists[i];
                Internal::s_metrics.vertices += cmd_list->VtxBuffer.Size;
                Internal::s_metrics.indices += cmd_list->IdxBuffer.Size;
            }
        }
    }

    inline void Render() {
        if (!Internal::s_initialized) return;
        
        // Render ImGui
        ImGui::Render();
        
        // Get framebuffer size
        int display_w, display_h;
        glfwGetFramebufferSize(Internal::s_window, &display_w, &display_h);
        
        // Clear screen
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Render ImGui draw data
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Handle multi-viewports (Updated for Docking branch)
        if (Internal::s_config.viewports_enabled) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        
        // Swap buffers
        glfwSwapBuffers(Internal::s_window);
    }

    inline void Shutdown() {
        if (!Internal::s_initialized) return;
        
        // Shutdown callback
        if (Internal::s_config.on_shutdown) {
            Internal::s_config.on_shutdown();
        }
        
        // Cleanup ImGui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        // Cleanup GLFW
        glfwDestroyWindow(Internal::s_window);
        glfwTerminate();
        
        // Reset state
        Internal::s_window = nullptr;
        Internal::s_initialized = false;
        Internal::s_should_close = false;
        Internal::s_metrics.Reset();
        
        std::cout << "[SHUTDOWN] Window system terminated" << std::endl;
    }

    // ============================================================================
    // UTILITY FUNCTIONS
    // ============================================================================

    inline GLFWwindow* GetWindow() { return Internal::s_window; }
    inline bool IsInitialized() { return Internal::s_initialized; }
    inline bool ShouldClose() { return Internal::s_should_close || (Internal::s_window && glfwWindowShouldClose(Internal::s_window)); }
    inline void SetShouldClose(bool should_close) { 
        Internal::s_should_close = should_close; 
        if (Internal::s_window) {
            glfwSetWindowShouldClose(Internal::s_window, should_close);
        }
    }
    
    inline const WindowConfig& GetConfig() { return Internal::s_config; }
    inline const PerformanceMetrics& GetMetrics() { return Internal::s_metrics; }
    
    inline ImFont* GetMainFont() { return Internal::s_main_font; }
    inline ImFont* GetBoldFont() { return Internal::s_bold_font; }
    inline ImFont* GetMonoFont() { return Internal::s_mono_font; }
    inline ImFont* GetIconFont() { return Internal::s_icon_font; }
    
    inline void SetWindowTitle(const std::string& title) {
        if (Internal::s_window) {
            glfwSetWindowTitle(Internal::s_window, title.c_str());
            Internal::s_config.title = title;
        }
    }
    
    inline void SetWindowSize(int width, int height) {
        if (Internal::s_window) {
            glfwSetWindowSize(Internal::s_window, width, height);
            Internal::s_config.width = width;
            Internal::s_config.height = height;
        }
    }
    
    inline void GetWindowSize(int& width, int& height) {
        if (Internal::s_window) {
            glfwGetWindowSize(Internal::s_window, &width, &height);
        }
    }
    
    inline void GetFramebufferSize(int& width, int& height) {
        if (Internal::s_window) {
            glfwGetFramebufferSize(Internal::s_window, &width, &height);
        }
    }
    
    inline void CenterWindow() {
        if (!Internal::s_window) return;
        
        GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* video_mode = glfwGetVideoMode(primary_monitor);
        
        int monitor_x, monitor_y;
        glfwGetMonitorPos(primary_monitor, &monitor_x, &monitor_y);
        
        int window_width, window_height;
        glfwGetWindowSize(Internal::s_window, &window_width, &window_height);
        
        int center_x = monitor_x + (video_mode->width - window_width) / 2;
        int center_y = monitor_y + (video_mode->height - window_height) / 2;
        
        glfwSetWindowPos(Internal::s_window, center_x, center_y);
    }
    
    inline void MaximizeWindow() {
        if (Internal::s_window) {
            glfwMaximizeWindow(Internal::s_window);
        }
    }
    
    inline void RestoreWindow() {
        if (Internal::s_window) {
            glfwRestoreWindow(Internal::s_window);
        }
    }
    
    inline void IconifyWindow() {
        if (Internal::s_window) {
            glfwIconifyWindow(Internal::s_window);
        }
    }

    // ============================================================================
    // DOCKSPACE HELPER
    // ============================================================================

    inline void BeginDockspace(const char* name = "MainDockspace", bool fullscreen = true) {
        if (!Internal::s_config.docking_enabled) return;
        
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        
        // UPDATED: Using correct ImGui Docking API
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        
        ImGuiWindowFlags window_flags = 
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoBackground;
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        
        ImGui::Begin(name, nullptr, window_flags);
        ImGui::PopStyleVar(3);
        
        ImGuiID dockspace_id = ImGui::GetID(name);
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 
                        fullscreen ? ImGuiDockNodeFlags_PassthruCentralNode : 0);
    }
    
    inline void EndDockspace() {
        ImGui::End();
    }

} // namespace WindowSetup

// ============================================================================
// CONVENIENCE MACROS
// ============================================================================

#define WINDOW_SETUP_QUICK_INIT(title) \
    WindowSetup::WindowConfig config; \
    config.title = title; \
    config.width = 1600; \
    config.height = 900; \
    config.fonts.load_font_awesome = true; \
    GLFWwindow* window = WindowSetup::Initialize(config); \
    if (!window) return 1;

#define WINDOW_SETUP_MAIN_LOOP \
    while (!WindowSetup::ShouldClose()) { \
        WindowSetup::BeginFrame(); \
        WindowSetup::BeginDockspace();

#define WINDOW_SETUP_END_LOOP \
        WindowSetup::EndDockspace(); \
        WindowSetup::EndFrame(); \
        WindowSetup::Render(); \
    } \
    WindowSetup::Shutdown();