#pragma once
#include "imgui.h"
#include "imgui_internal.h" 

// 1. Icons
#include "../Core/IconsFontAwesome6.h" 

// 2. WindowSetup - Direct include works now thanks to CMake
#include "WindowSetup.h"

// Fallback for safety
#ifndef ICON_FA_GEARS
#define ICON_FA_GEARS "[Settings]"
#endif

namespace UILab {

    struct DebugPanelState {
        bool showMetrics = false;
        bool showStack = false;
    };
    
    inline DebugPanelState g_DebugPanelState;
    
    inline void RenderDebugPanel() {
        if (ImGui::Begin("Lab Controls " ICON_FA_GEARS)) {
            ImGui::Text("Diagnostics");
            ImGui::Separator();
            
            // Performance Section
            if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
                // Public API for metrics
                ImGui::Text("Vertices: %d", ImGui::GetIO().MetricsRenderVertices);
                ImGui::Text("Indices:  %d", ImGui::GetIO().MetricsRenderIndices);
            }
            
            ImGui::Separator();
            ImGui::Text("Tools");
            ImGui::Checkbox("ImGui Metrics", &g_DebugPanelState.showMetrics);
            
            if (ImGui::Button("Stack Tool")) {
                 g_DebugPanelState.showStack = !g_DebugPanelState.showStack;
            }
            
            ImGui::Separator();
            ImGui::Text("Themes");
            
            // 3. Clean Theme Buttons
            if (ImGui::Button("Dark"))   WindowSetup::ApplyTheme(WindowSetup::ThemePreset::ClassicDark); 
            ImGui::SameLine();
            if (ImGui::Button("Light"))  WindowSetup::ApplyTheme(WindowSetup::ThemePreset::ClassicLight); 
            ImGui::SameLine();
            if (ImGui::Button("Cherry")) WindowSetup::ApplyTheme(WindowSetup::ThemePreset::Cherry);
        }
        ImGui::End();
    }
}