#include "menu.hpp"
#include "theme.hpp"
#include <algorithm>

// Helper used by both menu.cpp and this file
static ImU32 ColorRange(float i, float lo, float hi, ImU32 a, ImU32 b) {
    float t = (i - lo) / (hi - lo);
    t = std::clamp(t, 0.f, 1.f);
    ImVec4 ca = ImGui::ColorConvertU32ToFloat4(a);
    ImVec4 cb = ImGui::ColorConvertU32ToFloat4(b);
    return ImGui::ColorConvertFloat4ToU32(ImVec4(
        ca.x + (cb.x - ca.x) * t,
        ca.y + (cb.y - ca.y) * t,
        ca.z + (cb.z - ca.z) * t, 1.f));
}

// Maps a virtual (0..500, 0..600) menu coord into the preview canvas
struct PreviewCtx {
    ImDrawList* dl;
    ImVec2      origin;
    float       scale;
    float       fs;   // font size

    ImVec2 P(float x, float y) const {
        return ImVec2(origin.x + x * scale, origin.y + y * scale);
    }
    float S(float v) const { return v * scale; }

    void Text(float x, float y, ImU32 col, const char* str) const {
        dl->AddText(ImGui::GetFont(), fs, P(x, y), col, str);
    }
    ImVec2 TextSize(const char* str) const {
        ImVec2 s = ImGui::CalcTextSize(str);
        s.x *= scale; s.y *= scale;
        return s;
    }
};

void Menu::renderPreview(ImDrawList* dl, ImVec2 origin, float scale) {
    PreviewCtx c{ dl, origin, scale, 13.f * scale };

    // ---- window background ----
    dl->AddRectFilled(c.P(0, 0), c.P(500, 600), theme::bg());
    dl->AddRect(c.P(0, 0), c.P(500, 600), U32(0, 0, 0));

    // ---- title bar gradient ----
    dl->AddRectFilled(c.P(2, 2), c.P(498, 3), theme::accentU32());
    dl->AddRectFilled(c.P(2, 3), c.P(498, 4), theme::accentDarkU32());
    for (int i = 0; i < 20; i++) {
        ImU32 col = ColorRange((float)i, 0.f, 20.f, U32(50, 50, 50), U32(35, 35, 35));
        dl->AddRectFilled(c.P(2, 5 + i), c.P(498, 6 + i), col);
    }
    c.Text(6, 6, U32(255, 255, 255), menuName.c_str());

    // ---- inner body ----
    dl->AddRectFilled(c.P(2, 25), c.P(498, 598), theme::bg());
    dl->AddRectFilled(c.P(10, 24), c.P(490, 25), theme::accentU32());
    dl->AddRectFilled(c.P(10, 25), c.P(490, 26), theme::accentDarkU32());
    for (int i = 0; i < 15; i++) {
        ImU32 col = ColorRange((float)i, 0.f, 15.f, U32(50, 50, 50), U32(35, 35, 35));
        dl->AddRectFilled(c.P(10, 27 + i * 2), c.P(490, 29 + i * 2), col);
    }
    dl->AddRectFilled(c.P(10, 57), c.P(490, 598), theme::bg());

    // ---- tab bar ----
    if (!tabs.empty()) {
        float tabW = 480.f / (float)tabs.size();
        for (size_t i = 0; i < tabs.size(); i++) {
            float sx = 10.f + i * tabW;
            ImU32 bg = (i == (size_t)activeTab) ? U32(50, 50, 50) : U32(30, 30, 30);
            dl->AddRectFilled(c.P(sx, 27), c.P(sx + tabW, 59), bg);
            dl->AddRect(c.P(sx, 27), c.P(sx + tabW, 59), theme::border());

            ImVec2 ts = c.TextSize(tabs[i].name.c_str());
            c.Text(sx + (tabW - ts.x / scale) * 0.5f,
                   27 + (32 - ts.y / scale) * 0.5f,
                   (i == (size_t)activeTab) ? U32(255, 255, 255) : U32(170, 170, 170),
                   tabs[i].name.c_str());
        }
    }

    // ---- group boxes in current tab ----
    if (tabs.empty() || activeTab < 0 || activeTab >= (int)tabs.size()) return;
    Tab& tab = tabs[activeTab];

    const float COL_LEFT  = 17.f;
    const float COL_RIGHT = (500.f - 20.f) / 2.f + 13.f;
    const float COL_W     = (500.f - 40.f) / 2.f;
    const float TOP_Y     = 66.f;

    float yL = TOP_Y, yR = TOP_Y;

    for (auto& g : tab.groups) {
        bool  isRight = (g.autoPos == "right");
        float gx = (g.width > 0) ? g.x : (isRight ? COL_RIGHT : COL_LEFT);
        float gy = (g.y > 0)     ? g.y : (isRight ? yR        : yL);
        float gw = (g.width > 0) ? g.width : COL_W;

        // group box background + gradient header
        dl->AddRectFilled(c.P(gx, gy), c.P(gx + gw, gy + 30), theme::bg());
        dl->AddRect(c.P(gx, gy), c.P(gx + gw, gy + 30), U32(0, 0, 0));
        dl->AddRect(c.P(gx + 1, gy + 1), c.P(gx + gw - 1, gy + 29), theme::border());
        dl->AddRectFilled(c.P(gx + 2, gy + 2), c.P(gx + gw - 2, gy + 3), theme::accentU32());
        dl->AddRectFilled(c.P(gx + 2, gy + 3), c.P(gx + gw - 2, gy + 4), theme::accentDarkU32());
        if (!g.name.empty())
            c.Text(gx + 6, gy + 5, U32(255, 255, 255), g.name.c_str());

        // sub-tab strip if multigroup
        float widgetStartY = gy + 28.f;
        if (g.isMulti()) {
            float tx = gx + 2;
            for (size_t si = 0; si < g.subTabNames.size(); si++) {
                ImVec2 ts = c.TextSize(g.subTabNames[si].c_str());
                float bw = ts.x + 12.f * scale;
                ImU32 bg = (si == (size_t)g.activeSubTab) ? theme::accentU32() : U32(45, 45, 45);
                dl->AddRectFilled(c.P(tx / scale, gy + 28), c.P((tx + bw) / scale, gy + 48), bg);
                dl->AddText(ImGui::GetFont(), c.fs,
                            ImVec2(origin.x + tx + 6 * scale,
                                   origin.y + (gy + 30) * scale),
                            U32(255, 255, 255), g.subTabNames[si].c_str());
                tx += bw + 4 * scale;
            }
            widgetStartY = gy + 52.f;
        }

        // widgets (rendered as compact bars)
        auto& list = g.isMulti() ? g.subTabWidgets[g.activeSubTab] : g.widgets;
        float wy = widgetStartY;
        for (auto& w : list) {
            switch (w.type) {
            case WidgetType::Toggle: {
                dl->AddRectFilled(c.P(gx + 8, wy), c.P(gx + 20, wy + 12), U32(30, 30, 30));
                dl->AddRect(c.P(gx + 8, wy), c.P(gx + 20, wy + 12), U32(0, 0, 0));
                for (int k = 0; k < 4; k++) {
                    ImU32 col = w.boolValue
                        ? ColorRange((float)k, 0, 3, theme::accentU32(), theme::accentDarkU32())
                        : ColorRange((float)k, 0, 3, U32(50, 50, 50), U32(30, 30, 30));
                    dl->AddRectFilled(c.P(gx + 10, wy + 2 + k * 2),
                                      c.P(gx + 18, wy + 4 + k * 2), col);
                }
                c.Text(gx + 24, wy - 1, U32(255, 255, 255), w.name.c_str());
                if (w.hasColor) {
                    ImU32 cc = ImGui::ColorConvertFloat4ToU32(w.color);
                    dl->AddRectFilled(c.P(gx + gw - 46, wy - 1),
                                      c.P(gx + gw - 18, wy + 13), cc);
                }
                if (w.hasKeybind) {
                    dl->AddRectFilled(c.P(gx + gw - 52, wy), c.P(gx + gw - 8, wy + 16), U32(25, 25, 25));
                    dl->AddRect(c.P(gx + gw - 52, wy), c.P(gx + gw - 8, wy + 16), U32(30, 30, 30));
                    c.Text(gx + gw - 48, wy + 1, U32(255, 255, 255), "KEY");
                }
                wy += 18.f;
                break;
            }
            case WidgetType::Slider: {
                c.Text(gx + 8, wy - 3, U32(255, 255, 255), w.name.c_str());
                float barW = gw - 16;
                float t = (w.maxValue > w.minValue)
                    ? (w.floatValue - w.minValue) / (w.maxValue - w.minValue) : 0.f;
                for (int k = 0; k < 4; k++) {
                    ImU32 base = ColorRange((float)k, 0, 3, U32(50, 50, 50), U32(30, 30, 30));
                    dl->AddRectFilled(c.P(gx + 10, wy + 14 + k * 2),
                                      c.P(gx + 8 + barW, wy + 16 + k * 2), base);
                    ImU32 fill = ColorRange((float)k, 0, 3, theme::accentU32(), theme::accentDarkU32());
                    dl->AddRectFilled(c.P(gx + 10, wy + 14 + k * 2),
                                      c.P(gx + 10 + (barW - 4) * t, wy + 16 + k * 2), fill);
                }
                dl->AddRect(c.P(gx + 8, wy + 12), c.P(gx + 8 + barW, wy + 24), U32(30, 30, 30));
                wy += 32.f;
                break;
            }
            case WidgetType::Dropbox:
            case WidgetType::Combobox: {
                c.Text(gx + 8, wy - 3, U32(255, 255, 255), w.name.c_str());
                dl->AddRect(c.P(gx + 8, wy + 12), c.P(gx + gw - 8, wy + 34), U32(30, 30, 30));
                dl->AddRect(c.P(gx + 9, wy + 13), c.P(gx + gw - 9, wy + 33), U32(0, 0, 0));
                wy += 42.f;
                break;
            }
            case WidgetType::Textbox: {
                dl->AddRect(c.P(gx + 8, wy), c.P(gx + gw - 8, wy + 22), U32(30, 30, 30));
                dl->AddRect(c.P(gx + 9, wy + 1), c.P(gx + gw - 9, wy + 21), U32(0, 0, 0));
                c.Text(gx + 12, wy + 4, U32(255, 255, 255), w.textValue.c_str());
                wy += 30.f;
                break;
            }
            case WidgetType::Button: {
                dl->AddRect(c.P(gx + 8, wy), c.P(gx + gw - 8, wy + 22), U32(30, 30, 30));
                dl->AddRect(c.P(gx + 9, wy + 1), c.P(gx + gw - 9, wy + 21), U32(0, 0, 0));
                ImVec2 ts = c.TextSize(w.name.c_str());
                c.Text(gx + 8 + (gw - 16 - ts.x / scale) * 0.5f, wy + 4,
                       U32(255, 255, 255), w.name.c_str());
                wy += 28.f;
                break;
            }
            case WidgetType::List:
                wy += 22.f * 3.f + 22.f;
                break;
            }
        }

        float usedH = (wy - gy) + 8.f;
        dl->AddRect(c.P(gx, gy), c.P(gx + gw, gy + usedH), U32(0, 0, 0));

        if (isRight) yR = gy + usedH + 6;
        else         yL = gy + usedH + 6;
    }
}