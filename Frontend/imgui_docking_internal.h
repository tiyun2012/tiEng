#ifndef MY_IMGUI_DOCKING_INTERNAL_H
#define MY_IMGUI_DOCKING_INTERNAL_H

#include "imgui.h"
#include "imgui_internal.h"
#include <string>
#include <cstdio>

struct DockingSuggestor
{
    float threshold = 30.0f;
    bool active = false;
    std::string dragging_window_name;
    ImGuiID dockspace_id = 0;

    void BeginFrame(ImGuiID dockspace, float edgeThreshold = 30.0f) {
        dockspace_id = dockspace;
        threshold = edgeThreshold;
        if (!ImGui::IsMouseDown(0) && active) {
            active = false;
            dragging_window_name.clear();
        }
    }

    void WatchWindow(const char* window_name) {
        ImGuiContext& g = *GImGui;
        bool is_moving = (g.MovingWindow != NULL && strcmp(g.MovingWindow->Name, window_name) == 0);
        
        if (is_moving && !active) {
             active = true;
             dragging_window_name = window_name;
        } else if (!is_moving && active && dragging_window_name == window_name) {
             active = false;
             dragging_window_name.clear();
        }
    }

    void RenderAndHandle() {
        if (!active || dragging_window_name.empty()) return;

        ImGuiViewport* vp = ImGui::GetMainViewport();
        if (!vp) return;

        ImVec2 work_min = vp->WorkPos;
        ImVec2 work_max = ImVec2(work_min.x + vp->WorkSize.x, work_min.y + vp->WorkSize.y);
        
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        const ImU32 overlay_col = IM_COL32(0, 120, 215, 100);

        // Draw simple overlay for demo
        dl->AddRectFilled(work_min, work_max, overlay_col);
    }
};
#endif
