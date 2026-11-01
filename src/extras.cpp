#include "extras.hpp"
#include "theme.hpp"
#include <algorithm>
#include <cstdio>
#include <cmath>

// ──────────────────────────────────────────────────────────────────────────
//  WATERMARK
// ──────────────────────────────────────────────────────────────────────────
void Watermark::render(Menu& menu) {
    if (!menu.watermarkEnabled) return;
    ImGuiIO& io = ImGui::GetIO();

    char buf[256];
    snprintf(buf, sizeof buf, "%s | %d fps | %d ms | %s",
        menu.menuName.c_str(),
        (int)(io.Framerate + 0.5f),
        (int)(menu.ping + 0.5f),
        "Nov 1, 2026");

    ImVec2 tsz = ImGui::CalcTextSize(buf);
    float w = tsz.x + 12, h = 20;

    ImGui::SetNextWindowPos(ImVec2(50, 9), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(w, h));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, theme::bg());          // ImU32 overload
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0,0,0,0));
    ImGui::Begin("##wm", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetWindowPos();
    dl->AddRectFilled(ImVec2(p.x, p.y+1), ImVec2(p.x+w, p.y+3), theme::accentDarkU32());
    dl->AddRectFilled(ImVec2(p.x, p.y),   ImVec2(p.x+w, p.y+2), theme::accentU32());
    for (int i = 0; i < (int)h-5; i++) {
        int c = (int)(50 - i * 1.7f);
        dl->AddRectFilled(ImVec2(p.x, p.y+3+i), ImVec2(p.x+w, p.y+4+i), U32(c,c,c));
    }
    dl->AddRect(ImVec2(p.x, p.y), ImVec2(p.x+w, p.y+h), U32(0,0,0));
    dl->AddText(ImVec2(p.x+5, p.y+3), U32(255,255,255), buf);

    ImGui::End();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();
}

// ──────────────────────────────────────────────────────────────────────────
//  KEYBIND HUD
// ──────────────────────────────────────────────────────────────────────────
void KeybindHUD::render(Menu& menu) {
    if (!menu.keybindHudEnabled) return;
    auto binds = menu.getActiveKeybinds();
    if (binds.empty()) return;

    float rowH = 16.f, padTop = 4.f, padBot = 4.f;
    float w = 120.f;
    for (auto& b : binds) {
        float bw = ImGui::CalcTextSize((b.name + "  [" + b.key + "]").c_str()).x + 12;
        if (bw > w) w = bw;
    }
    float h = binds.size() * rowH + padTop + padBot;

    ImGui::SetNextWindowPos(ImVec2(50, 200), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0,0,0,0));
    ImGui::Begin("##kbhud", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetWindowPos();
    dl->AddRectFilled(ImVec2(p.x, p.y+1), ImVec2(p.x+w, p.y+3), theme::accentDarkU32());
    dl->AddRectFilled(ImVec2(p.x, p.y),   ImVec2(p.x+w, p.y+2), theme::accentU32());
    for (int i = 0; i < (int)(h-4); i++) {
        int c = (int)(50 - i * 0.15f);
        dl->AddRectFilled(ImVec2(p.x, p.y+3+i), ImVec2(p.x+w, p.y+4+i), U32(c,c,c));
    }
    dl->AddRect(ImVec2(p.x, p.y), ImVec2(p.x+w, p.y+h), U32(0,0,0));

    for (size_t i = 0; i < binds.size(); i++) {
        std::string s = binds[i].name + "  [" + binds[i].key + "]";
        dl->AddText(ImVec2(p.x+6, p.y+4+i*rowH), U32(255,255,255), s.c_str());
    }

    ImGui::End();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();
}

// ──────────────────────────────────────────────────────────────────────────
//  PREVIEW WINDOW  —  host window; delegates drawing to menu.renderPreview
// ──────────────────────────────────────────────────────────────────────────
void Preview::render(Menu& menu) {
    if (!visible) return;

    ImGui::SetNextWindowSize(ImVec2(340, 460), ImGuiCond_FirstUseEver);
    ImGui::PushStyleColor(ImGuiCol_WindowBg,      ImVec4(0.12f,0.12f,0.12f,1));
    ImGui::PushStyleColor(ImGuiCol_TitleBg,       V4(45,45,45));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, V4(55,55,55));
    ImGui::Begin("Menu Preview", &visible);

    ImGui::TextUnformatted("Live preview - reflects the actual menu state");
    ImGui::Separator();

    ImVec2 avail  = ImGui::GetContentRegionAvail();
    float  scale  = std::min(avail.x / 500.f, avail.y / 600.f) * 0.95f;
    if (scale < 0.05f) { ImGui::End(); ImGui::PopStyleColor(3); return; }

    ImVec2 previewSize(500.f * scale, 600.f * scale);

    ImVec2 origin = ImGui::GetCursorScreenPos();
    origin.x += (avail.x - previewSize.x) * 0.5f;

    ImGui::InvisibleButton("##previewcanvas", previewSize);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    menu.renderPreview(dl, origin, scale);

    ImGui::End();
    ImGui::PopStyleColor(3);
}