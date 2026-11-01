#include "theme.hpp"

void theme::applyStyle() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding    = 0.f;
    s.FrameRounding     = 2.f;
    s.PopupRounding     = 2.f;
    s.ScrollbarRounding = 2.f;
    s.GrabRounding      = 2.f;
    s.TabRounding       = 2.f;
    s.WindowBorderSize  = 2.f;
    s.FrameBorderSize   = 1.f;
    s.WindowPadding     = ImVec2(8, 8);
    s.FramePadding      = ImVec2(6, 3);
    s.ItemSpacing       = ImVec2(6, 4);

    auto& c = s.Colors;
    c[ImGuiCol_Text]                  = V4(255, 255, 255);
    c[ImGuiCol_TextDisabled]          = V4(170, 170, 170);
    c[ImGuiCol_WindowBg]              = V4(35, 35, 35);
    c[ImGuiCol_ChildBg]               = V4(30, 30, 30);
    c[ImGuiCol_PopupBg]               = V4(30, 30, 30, 250);
    c[ImGuiCol_Border]                = V4(20, 20, 20);
    c[ImGuiCol_BorderShadow]          = V4(0, 0, 0, 0);
    c[ImGuiCol_FrameBg]               = V4(30, 30, 30);
    c[ImGuiCol_FrameBgHovered]        = V4(45, 45, 45);
    c[ImGuiCol_FrameBgActive]         = V4(50, 50, 50);
    c[ImGuiCol_TitleBg]               = V4(35, 35, 35);
    c[ImGuiCol_TitleBgActive]         = V4(50, 50, 50);
    c[ImGuiCol_TitleBgCollapsed]      = V4(35, 35, 35);
    c[ImGuiCol_MenuBarBg]             = V4(30, 30, 30);
    c[ImGuiCol_ScrollbarBg]           = V4(20, 20, 20);
    c[ImGuiCol_ScrollbarGrab]         = V4(50, 50, 50);
    c[ImGuiCol_ScrollbarGrabHovered]  = V4(70, 70, 70);
    c[ImGuiCol_ScrollbarGrabActive]   = V4(127, 72, 163);
    c[ImGuiCol_CheckMark]             = V4(127, 72, 163);
    c[ImGuiCol_SliderGrab]            = V4(127, 72, 163);
    c[ImGuiCol_SliderGrabActive]      = V4(87, 32, 123);
    c[ImGuiCol_Button]                = V4(50, 50, 50);
    c[ImGuiCol_ButtonHovered]         = V4(70, 70, 70);
    c[ImGuiCol_ButtonActive]          = V4(127, 72, 163);
    c[ImGuiCol_Header]                = V4(50, 50, 50);
    c[ImGuiCol_HeaderHovered]         = V4(70, 70, 70);
    c[ImGuiCol_HeaderActive]          = V4(127, 72, 163);
    c[ImGuiCol_Separator]             = V4(20, 20, 20);
    c[ImGuiCol_SeparatorHovered]      = V4(87, 32, 123);
    c[ImGuiCol_SeparatorActive]       = V4(127, 72, 163);
    c[ImGuiCol_ResizeGrip]            = V4(50, 50, 50);
    c[ImGuiCol_ResizeGripHovered]     = V4(127, 72, 163);
    c[ImGuiCol_ResizeGripActive]      = V4(87, 32, 123);
    c[ImGuiCol_Tab]                   = V4(35, 35, 35);
    c[ImGuiCol_TabHovered]            = V4(60, 60, 60);
    #if IMGUI_VERSION_NUM >= 19090
        c[ImGuiCol_TabSelected] = V4(50, 50, 50);
        c[ImGuiCol_TabDimmed] = V4(30, 30, 30);
        c[ImGuiCol_TabDimmedSelected] = V4(45, 45, 45);
    #else
        c[ImGuiCol_TabActive] = V4(50, 50, 50);
        c[ImGuiCol_TabUnfocused] = V4(30, 30, 30);
        c[ImGuiCol_TabUnfocusedActive] = V4(45, 45, 45);
    #endif
    c[ImGuiCol_PlotLines]             = V4(127, 72, 163);
    c[ImGuiCol_PlotHistogram]         = V4(127, 72, 163);
    c[ImGuiCol_TableHeaderBg]         = V4(30, 30, 30);
    c[ImGuiCol_TableBorderStrong]     = V4(0, 0, 0);
    c[ImGuiCol_TableBorderLight]      = V4(20, 20, 20);
    c[ImGuiCol_TextSelectedBg]        = V4(127, 72, 163, 128);
    c[ImGuiCol_NavHighlight]          = V4(127, 72, 163);
    c[ImGuiCol_NavWindowingHighlight] = V4(255, 255, 255);
    c[ImGuiCol_NavWindowingDimBg]     = V4(0, 0, 0, 100);
    c[ImGuiCol_ModalWindowDimBg]      = V4(0, 0, 0, 150);
}