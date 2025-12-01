#pragma once

#include "imgui.h"

namespace UILab {
    
    // Common UI utilities
    inline bool BeginCenteredWindow(const char* name, bool* p_open = nullptr) {
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), 
                               ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        return ImGui::Begin(name, p_open, ImGuiWindowFlags_AlwaysAutoResize);
    }
    
    inline void HelpMarker(const char* desc) {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
    
} // namespace UILab