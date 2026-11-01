#pragma once
#include "imgui.h"

inline ImVec4 V4(int r, int g, int b, int a = 255) {
    return ImVec4(r/255.f, g/255.f, b/255.f, a/255.f);
}
inline ImU32 U32(int r, int g, int b, int a = 255) {
    return IM_COL32(r, g, b, a);
}

namespace theme {
    inline ImVec4 accent     = V4(127, 72, 163);
    inline ImVec4 accentDark = V4(87, 32, 123);
    inline ImU32 accentU32()     { return ImGui::ColorConvertFloat4ToU32(accent); }
    inline ImU32 accentDarkU32() { return ImGui::ColorConvertFloat4ToU32(accentDark); }
    inline ImU32 bg()      { return U32(35, 35, 35); }
    inline ImU32 bgLight() { return U32(50, 50, 50); }
    inline ImU32 panel()   { return U32(30, 30, 30); }
    inline ImU32 border()  { return U32(20, 20, 20); }
    inline ImU32 border2() { return U32(0, 0, 0); }
    inline ImU32 text()    { return U32(255, 255, 255); }
    inline ImU32 textDim() { return U32(170, 170, 170); }
    void applyStyle();
}