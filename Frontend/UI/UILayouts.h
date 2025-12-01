#pragma once

#include "Components/DebugPanel.h"
#include "Components/Inspector.h"

namespace UILab {

    // Main Entry Point
    inline void Render() {
        RenderDebugPanel();
        RenderInspector();

        // Debug windows (conditionally rendered)
        if (g_DebugPanelState.showMetrics) {
            ImGui::ShowMetricsWindow(&g_DebugPanelState.showMetrics);
        }
        if (g_DebugPanelState.showStack) {
            ImGui::ShowIDStackToolWindow(&g_DebugPanelState.showStack);
        }
    }
    
} // namespace UILab