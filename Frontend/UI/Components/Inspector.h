#pragma once
#include "imgui.h"
#include "../Core/IconsFontAwesome6.h"
#include <string>

namespace UILab {

    struct InspectorState {
        std::string objectName = "Player_01";
        float position[3] = {0, 10, 0};
    };
    
    inline InspectorState g_InspectorState;
    
    inline void RenderInspector() {
        ImGui::Begin("Inspector " ICON_FA_MAGNIFYING_GLASS);
        
        ImGui::Text("Object Properties");
        ImGui::Separator();
        
        // Requires <cstring> for cross-platform compatibility
        char buf[64];
        strncpy(buf, g_InspectorState.objectName.c_str(), sizeof(buf));
        buf[sizeof(buf) - 1] = '\0'; // Ensure null termination
        if (ImGui::InputText("Name", buf, sizeof(buf))) {
            g_InspectorState.objectName = buf;
        }
        ImGui::DragFloat3("Position", g_InspectorState.position, 0.1f);
        
        if(ImGui::Button(ICON_FA_FLOPPY_DISK " Save Asset")) {
            ImGui::OpenPopup("Saved");
        }

        if(ImGui::BeginPopup("Saved")) {
            ImGui::Text("Data saved to disk!");
            ImGui::EndPopup();
        }

        ImGui::End();
    }
    
} // namespace UILab