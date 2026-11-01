#include "menu.hpp"
#include "theme.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdio>

namespace fs = std::filesystem;

static const float COL_LEFT  = 17.f;
static const float COL_RIGHT = (500.f - 20.f) / 2.f + 13.f;
static const float COL_W     = (500.f - 40.f) / 2.f;
static const float TAB_H     = 32.f;
static const float TOP_Y     = 66.f;

static ImU32 ColorRange(float i, float lo, float hi, ImU32 a, ImU32 b) {
    float t = std::clamp((i - lo) / (hi - lo), 0.f, 1.f);
    ImVec4 ca = ImGui::ColorConvertU32ToFloat4(a);
    ImVec4 cb = ImGui::ColorConvertU32ToFloat4(b);
    return ImGui::ColorConvertFloat4ToU32(ImVec4(
        ca.x + (cb.x - ca.x) * t,
        ca.y + (cb.y - ca.y) * t,
        ca.z + (cb.z - ca.z) * t, 1.f));
}

static float widgetHeight(const Widget& w) {
    switch (w.type) {
        case WidgetType::Toggle:   return 18.f;
        case WidgetType::Slider:   return 32.f;
        case WidgetType::Dropbox:  return 42.f;
        case WidgetType::Combobox: return 42.f;
        case WidgetType::Textbox:  return 30.f;
        case WidgetType::Button:   return 28.f;
        case WidgetType::List:     return 22.f * 4.f;
    }
    return 20.f;
}

static const char* keyName(ImGuiKey k) {
    if (k == ImGuiKey_None) return "None";
    const char* s = ImGui::GetKeyName(k);
    return s ? s : "?";
}

// Demo content — mirrors the original Lua demo menu
void Menu::buildDemo() {
    tabs.clear();
    Tab demoTab{"Demo"};

    {   // Toggles group (left)
        Group g; g.name = "Toggles"; g.autoPos = "left";
        Widget w; w.name = "Basic Toggle"; g.widgets.push_back(w);
        w = {}; w.name = "Toggle with Keybind";
        w.hasKeybind = true; w.keyCode = ImGuiKey_X; w.keyToggleMode = ToggleMode::Toggle;
        g.widgets.push_back(w);
        w = {}; w.name = "Toggle with Color";
        w.hasColor = true; w.color = V4(255,0,128);
        g.widgets.push_back(w);
        w = {}; w.name = "Toggle Dual Color";
        w.hasColor = true; w.color = V4(255,0,0);
        w.hasDoubleColor = true; w.color2 = V4(0,0,255);
        g.widgets.push_back(w);
        w = {}; w.name = "Unsafe Toggle"; w.unsafe = true;
        w.tooltip = "Requires Allow Unsafe Features."; g.widgets.push_back(w);
        w = {}; w.name = "Toggle with Tooltip";
        w.tooltip = "This is a helpful tooltip."; g.widgets.push_back(w);
        demoTab.groups.push_back(g);
    }

    {   // Inputs group (right)
        Group g; g.name = "Inputs"; g.autoPos = "right";
        Widget w; w.type = WidgetType::Slider; w.name = "Basic Slider";
        w.floatValue = 50; w.minValue = 0; w.maxValue = 100; w.suffix = "%";
        g.widgets.push_back(w);
        w = {}; w.type = WidgetType::Slider; w.name = "Decimal Slider";
        w.floatValue = 1.f; w.minValue = 0; w.maxValue = 5; w.decimal = 0.1f; w.suffix = "x";
        g.widgets.push_back(w);
        w = {}; w.type = WidgetType::Slider; w.name = "Custom Labels";
        w.floatValue = 0; w.minValue = 0; w.maxValue = 3; w.step = 1;
        g.widgets.push_back(w);
        w = {}; w.type = WidgetType::Dropbox; w.name = "Dropbox";
        w.intValue = 1; w.options = {"Option A","Option B","Option C"};
        g.widgets.push_back(w);
        w = {}; w.type = WidgetType::Combobox; w.name = "Combobox";
        w.multiOptions = { {"Choice 1", true}, {"Choice 2", false}, {"Choice 3", true} };
        g.widgets.push_back(w);
        w = {}; w.type = WidgetType::Textbox; w.name = "Text Input";
        w.textValue = "default";
        g.widgets.push_back(w);
        demoTab.groups.push_back(g);
    }

    {   // Buttons (left, autofill)
        Group g; g.name = "Buttons"; g.autoPos = "left"; g.autofill = true;
        Widget w; w.type = WidgetType::Button; w.name = "Basic Button";
        g.widgets.push_back(w);
        w = {}; w.type = WidgetType::Button; w.name = "Reset"; w.doubleClick = true;
        g.widgets.push_back(w);
        demoTab.groups.push_back(g);
    }

    {   // Tab A / B multigroup (right, autofill)
        Group g; g.autoPos = "right"; g.autofill = true;
        g.subTabNames = { "Tab A", "Tab B" };
        g.subTabWidgets.resize(2);
        Widget w; w.name = "Walkspeed";
        w.hasKeybind = true; w.keyCode = ImGuiKey_V; w.keyToggleMode = ToggleMode::Toggle;
        g.subTabWidgets[0].push_back(w);
        w = {}; w.type = WidgetType::Slider; w.name = "Walkspeed Value";
        w.floatValue = 16; w.minValue = 0; w.maxValue = 100;
        g.subTabWidgets[0].push_back(w);
        w = {}; w.name = "Feature in Tab B"; w.boolValue = true;
        g.subTabWidgets[1].push_back(w);
        w = {}; w.type = WidgetType::Dropbox; w.name = "Dropbox in Tab B";
        w.intValue = 1; w.options = {"X","Y","Z"};
        g.subTabWidgets[1].push_back(w);
        demoTab.groups.push_back(g);
    }

    tabs.push_back(demoTab);

    Tab setTab{"Settings"};
    {   // Cheat Settings
        Group g; g.name = "Cheat Settings";
        g.x = COL_LEFT; g.y = 78.f; g.width = COL_W; g.height = 220.f;
        Widget w; w.name = "Menu Accent";
        w.hasColor = true; w.color = V4(127,72,163); g.widgets.push_back(w);
        w = {}; w.name = "Watermark"; w.boolValue = true; g.widgets.push_back(w);
        w = {}; w.name = "Custom Menu Name"; g.widgets.push_back(w);
        w = {}; w.type = WidgetType::Textbox; w.name = "MenuName";
        w.textValue = "Bitch Bot"; g.widgets.push_back(w);
        w = {}; w.type = WidgetType::Button; w.name = "Set Clipboard Game ID";
        g.widgets.push_back(w);
        w = {}; w.type = WidgetType::Button; w.name = "Unload Cheat"; w.doubleClick = true;
        g.widgets.push_back(w);
        w = {}; w.name = "Allow Unsafe Features"; g.widgets.push_back(w);
        w = {}; w.name = "Menu Key"; w.boolValue = true;
        w.hasKeybind = true; w.keyCode = ImGuiKey_Delete; w.keyToggleMode = ToggleMode::Toggle;
        g.widgets.push_back(w);
        w = {}; w.name = "Keybind List"; w.boolValue = true; g.widgets.push_back(w);
        setTab.groups.push_back(g);
    }
    {   // Configuration
        Group g; g.name = "Configuration";
        g.x = COL_RIGHT; g.y = 100.f; g.width = COL_W; g.height = 182.f;
        Widget w; w.type = WidgetType::Textbox; w.name = "ConfigName";
        w.textValue = "Default"; w.isFile = true; g.widgets.push_back(w);
        w = {}; w.type = WidgetType::Dropbox; w.name = "Configs";
        w.intValue = 1; w.options = listConfigs();
        if (w.options.empty()) w.options.push_back("Default");
        g.widgets.push_back(w);
        w = {}; w.type = WidgetType::Button; w.name = "Load Config"; w.doubleClick = true;
        g.widgets.push_back(w);
        w = {}; w.type = WidgetType::Button; w.name = "Save Config"; w.doubleClick = true;
        g.widgets.push_back(w);
        w = {}; w.type = WidgetType::Button; w.name = "Delete Config"; w.doubleClick = true;
        g.widgets.push_back(w);
        setTab.groups.push_back(g);
    }
    tabs.push_back(setTab);

    // wire callbacks
    for (auto& t : tabs)
        for (auto& g : t.groups) {
            auto wire = [&](Widget& w) {
                std::string tn = t.name, gn = g.name, wn = w.name;
                w.onButton = [this, tn, gn, wn](Widget&) {
                    printf("[Button] %s > %s > %s pressed\n", tn.c_str(), gn.c_str(), wn.c_str());
                    if (wn == "Save Config") {
                        auto* cfg = find("Settings","Configuration","ConfigName");
                        if (cfg) { saveConfig(cfg->textValue); notify("Saved: " + cfg->textValue); }
                    } else if (wn == "Load Config") {
                        auto* cfgName = find("Settings","Configuration","ConfigName");
                        auto* cfg = find("Settings","Configuration","Configs");
                        if (cfgName && cfg && cfg->intValue >= 1 && cfg->intValue <= (int)cfg->options.size()) {
                            cfgName->textValue = cfg->options[cfg->intValue-1];
                            loadConfig(cfgName->textValue);
                            notify("Loaded: " + cfgName->textValue);
                        }
                    } else if (wn == "Delete Config") {
                        auto* cfg = find("Settings","Configuration","ConfigName");
                        if (cfg) {
                            std::error_code ec;
                            fs::remove("bitchbot/" + game + "/" + cfg->textValue + ".bb", ec);
                            notify("Deleted: " + cfg->textValue);
                        }
                    }
                };
                w.onChange = [this, tn, gn, wn](Widget& ww) {
                    if (ww.type == WidgetType::Toggle)
                        printf("[Toggle] %s > %s > %s = %s\n", tn.c_str(), gn.c_str(), wn.c_str(), ww.boolValue?"true":"false");
                };
            };
            for (auto& w : g.widgets) wire(w);
            for (auto& v : g.subTabWidgets) for (auto& w : v) wire(w);
        }
}

Widget* Menu::find(const std::string& tn, const std::string& gn, const std::string& wn) {
    for (auto& t : tabs) {
        if (t.name != tn) continue;
        for (auto& g : t.groups) {
            if (g.name != gn) continue;
            for (auto& w : g.widgets) if (w.name == wn) return &w;
            for (auto& v : g.subTabWidgets) for (auto& w : v) if (w.name == wn) return &w;
        }
    }
    return nullptr;
}

void Menu::drawWidget(Widget& w, float wx, float wy, float ww) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 org = ImGui::GetWindowPos();
    float sx = org.x + wx, sy = org.y + wy;

    ImGui::PushID(&w);
    switch (w.type) {
    case WidgetType::Toggle: {
        dl->AddRectFilled(ImVec2(sx, sy), ImVec2(sx+12, sy+12), U32(30,30,30));
        dl->AddRect(ImVec2(sx, sy), ImVec2(sx+12, sy+12), U32(0,0,0));
        dl->AddRectFilled(ImVec2(sx+1, sy+1), ImVec2(sx+11, sy+11), U32(0,0,0));
        for (int i = 0; i < 4; i++) {
            ImU32 c = w.boolValue
                ? ColorRange((float)i, 0, 3, theme::accentU32(), theme::accentDarkU32())
                : ColorRange((float)i, 0, 3, U32(50,50,50), U32(30,30,30));
            dl->AddRectFilled(ImVec2(sx+2, sy+2+i*2), ImVec2(sx+10, sy+4+i*2), c);
        }
        dl->AddText(ImVec2(sx+16, sy-1),
                    w.unsafe ? U32(90,90,90) : U32(255,255,255), w.name.c_str());

        ImGui::SetCursorScreenPos(ImVec2(sx, sy));
        ImGui::InvisibleButton("##tog", ImVec2(16 + ImGui::CalcTextSize(w.name.c_str()).x, 16));
        if (ImGui::IsItemClicked()) {
            if (w.unsafe) {
                auto* allow = find("Settings","Cheat Settings","Allow Unsafe Features");
                if (allow && !allow->boolValue) { ImGui::PopID(); return; }
            }
            w.boolValue = !w.boolValue;
            if (w.onChange) w.onChange(w);
        }
        if (ImGui::IsItemHovered() && !w.tooltip.empty()) ImGui::SetTooltip("%s", w.tooltip.c_str());

        if (w.hasKeybind) {
            float kbx = sx + ww - 44, kby = sy;
            dl->AddRectFilled(ImVec2(kbx, kby), ImVec2(kbx+44, kby+16), U32(25,25,25));
            const char* kn = keyName(w.keyCode);
            ImVec2 ts = ImGui::CalcTextSize(kn);
            dl->AddText(ImVec2(kbx+(44-ts.x)/2, kby+(16-ts.y)/2-1), U32(255,255,255), kn);
            dl->AddRect(ImVec2(kbx, kby), ImVec2(kbx+44, kby+16), U32(30,30,30));
            dl->AddRect(ImVec2(kbx+1, kby+1), ImVec2(kbx+43, kby+15), U32(0,0,0));
            ImGui::SetCursorScreenPos(ImVec2(kbx, kby));
            ImGui::InvisibleButton("##kb", ImVec2(44, 16));
            if (ImGui::IsItemClicked()) w.listeningForKey = true;
            if (w.listeningForKey) {
                for (int ki = (int)ImGuiKey_NamedKey_BEGIN; ki < (int)ImGuiKey_NamedKey_END; ++ki) {
                    ImGuiKey k = (ImGuiKey)ki;
                    if (ImGui::IsKeyPressed(k, false)) {
                        w.keyCode = k;
                        w.listeningForKey = false;
                        break;
                    }
                }
            }
        }
        if (w.hasColor) {
            int n = w.hasDoubleColor ? 2 : 1;
            for (int i = 0; i < n; i++) {
                ImVec4 c = (i == 0) ? w.color : w.color2;
                float cx = sx + ww - 38 - i*34, cy = sy - 1;
                dl->AddRect(ImVec2(cx, cy), ImVec2(cx+28, cy+14), U32(30,30,30));
                dl->AddRect(ImVec2(cx+1, cy+1), ImVec2(cx+27, cy+13), U32(0,0,0));
                dl->AddRectFilled(ImVec2(cx+2, cy+2), ImVec2(cx+26, cy+12),
                                  ImGui::ColorConvertFloat4ToU32(c));
                ImGui::SetCursorScreenPos(ImVec2(cx, cy));
                ImGui::InvisibleButton(("##cp" + std::to_string(i)).c_str(), ImVec2(28,14));
                if (ImGui::IsItemClicked()) ImGui::OpenPopup(("cpPopup" + std::to_string(i)).c_str());
                if (ImGui::BeginPopup(("cpPopup" + std::to_string(i)).c_str())) {
                    float* ptr = (i == 0) ? &w.color.x : &w.color2.x;
                    ImGui::ColorPicker4("##pick", ptr, ImGuiColorEditFlags_AlphaBar);
                    ImGui::EndPopup();
                }
            }
        }
        break;
    }
    case WidgetType::Slider: {
        dl->AddText(ImVec2(sx, sy-3), U32(255,255,255), w.name.c_str());
        float t = (w.maxValue > w.minValue) ? (w.floatValue-w.minValue)/(w.maxValue-w.minValue) : 0.f;
        float barY = sy + 14, barW = ww;
        for (int i = 0; i < 4; i++) {
            dl->AddRectFilled(ImVec2(sx+2, barY+i*2), ImVec2(sx+barW-2, barY+i*2+2), U32(0,0,0));
            dl->AddRectFilled(ImVec2(sx+2, barY+i*2), ImVec2(sx+barW-2, barY+i*2+2),
                ColorRange((float)i, 0, 3, U32(50,50,50), U32(30,30,30)));
        }
        for (int i = 0; i < 4; i++) {
            dl->AddRectFilled(ImVec2(sx+2, barY+i*2), ImVec2(sx+2+(barW-4)*t, barY+i*2+2),
                ColorRange((float)i, 0, 3, theme::accentU32(), theme::accentDarkU32()));
        }
        dl->AddRect(ImVec2(sx, sy+12), ImVec2(sx+barW, sy+24), U32(30,30,30));
        dl->AddRect(ImVec2(sx+1, sy+13), ImVec2(sx+barW-1, sy+23), U32(0,0,0));
        char buf[64];
        if (w.decimal > 0) snprintf(buf, sizeof buf, "%.2f%s", w.floatValue, w.suffix.c_str());
        else snprintf(buf, sizeof buf, "%d%s", (int)w.floatValue, w.suffix.c_str());
        ImVec2 ts = ImGui::CalcTextSize(buf);
        dl->AddText(ImVec2(sx+(ww-ts.x)/2, sy+11), U32(255,255,255), buf);

        ImGui::SetCursorScreenPos(ImVec2(sx, sy));
        ImGui::InvisibleButton("##sl", ImVec2(ww, 28));
        bool active = ImGui::IsItemActive();
        if ((ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) || (active && ImGui::IsMouseDragging(0))) {
            float mx = ImGui::GetIO().MousePos.x;
            float tt = std::clamp((mx - sx - 2) / (barW - 4), 0.f, 1.f);
            float nv = w.minValue + tt * (w.maxValue - w.minValue);
            nv = w.decimal > 0 ? std::floor(nv / w.decimal) * w.decimal : std::floor(nv);
            w.floatValue = std::clamp(nv, w.minValue, w.maxValue);
            if (w.onChange) w.onChange(w);
        }
        break;
    }
    case WidgetType::Dropbox: {
        dl->AddText(ImVec2(sx, sy-3), U32(255,255,255), w.name.c_str());
        for (int i = 0; i < 8; i++) {
            ImU32 c = ColorRange((float)i, 0, 7, U32(50,50,50), U32(35,35,35));
            dl->AddRectFilled(ImVec2(sx+2, sy+14+i*2), ImVec2(sx+ww-2, sy+16+i*2), c);
        }
        dl->AddRect(ImVec2(sx, sy+12), ImVec2(sx+ww, sy+34), U32(30,30,30));
        dl->AddRect(ImVec2(sx+1, sy+13), ImVec2(sx+ww-1, sy+33), U32(0,0,0));
        const char* cur = (w.intValue >= 1 && w.intValue <= (int)w.options.size())
                          ? w.options[w.intValue-1].c_str() : "None";
        dl->AddText(ImVec2(sx+6, sy+16), U32(255,255,255), cur);
        dl->AddText(ImVec2(sx+ww-14, sy+16), U32(255,255,255), "-");
        ImGui::SetCursorScreenPos(ImVec2(sx, sy+12));
        ImGui::InvisibleButton("##dr", ImVec2(ww, 22));
        if (ImGui::IsItemClicked()) ImGui::OpenPopup("##dropPopup");
        if (ImGui::BeginPopup("##dropPopup")) {
            for (int i = 1; i <= (int)w.options.size(); i++) {
                if (ImGui::Selectable(w.options[i-1].c_str(), i == w.intValue)) {
                    w.intValue = i;
                    if (w.onChange) w.onChange(w);
                }
            }
            ImGui::EndPopup();
        }
        break;
    }
    case WidgetType::Combobox: {
        dl->AddText(ImVec2(sx, sy-3), U32(255,255,255), w.name.c_str());
        for (int i = 0; i < 8; i++) {
            ImU32 c = ColorRange((float)i, 0, 7, U32(50,50,50), U32(35,35,35));
            dl->AddRectFilled(ImVec2(sx+2, sy+14+i*2), ImVec2(sx+ww-2, sy+16+i*2), c);
        }
        dl->AddRect(ImVec2(sx, sy+12), ImVec2(sx+ww, sy+34), U32(30,30,30));
        dl->AddRect(ImVec2(sx+1, sy+13), ImVec2(sx+ww-1, sy+33), U32(0,0,0));
        std::string joined;
        for (auto& [nm, on] : w.multiOptions)
            if (on) { if (!joined.empty()) joined += ", "; joined += nm; }
        if (joined.empty()) joined = "None";
        if (joined.size() > 25) joined = joined.substr(0, 25);
        dl->AddText(ImVec2(sx+6, sy+16), U32(255,255,255), joined.c_str());
        dl->AddText(ImVec2(sx+ww-20, sy+16), U32(255,255,255), "...");
        ImGui::SetCursorScreenPos(ImVec2(sx, sy+12));
        ImGui::InvisibleButton("##cb", ImVec2(ww, 22));
        if (ImGui::IsItemClicked()) ImGui::OpenPopup("##cbPopup");
        if (ImGui::BeginPopup("##cbPopup")) {
            for (auto& [nm, on] : w.multiOptions)
                if (ImGui::Checkbox(nm.c_str(), &on) && w.onChange) w.onChange(w);
            ImGui::EndPopup();
        }
        break;
    }
    case WidgetType::Textbox: {
        for (int i = 0; i < 8; i++) {
            ImU32 c = ColorRange((float)i, 0, 7, U32(50,50,50), U32(35,35,35));
            dl->AddRectFilled(ImVec2(sx+2, sy+2+i*2), ImVec2(sx+ww-2, sy+4+i*2), c);
        }
        dl->AddRect(ImVec2(sx, sy), ImVec2(sx+ww, sy+22), U32(30,30,30));
        dl->AddRect(ImVec2(sx+1, sy+1), ImVec2(sx+ww-1, sy+21), U32(0,0,0));
        ImGui::SetCursorScreenPos(ImVec2(sx+4, sy+3));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0,0,0,0));
        ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0,0,0,0));
        ImGui::SetNextItemWidth(ww-8);
        char buf[256];
        strncpy(buf, w.textValue.c_str(), sizeof buf - 1); buf[sizeof buf - 1] = 0;
        if (ImGui::InputText(("##tx"+w.name).c_str(), buf, sizeof buf)) {
            w.textValue = buf;
            if (w.onChange) w.onChange(w);
        }
        ImGui::PopStyleColor(2);
        break;
    }
    case WidgetType::Button: {
        for (int i = 0; i < 8; i++) {
            ImU32 c = ColorRange((float)i, 0, 7, U32(50,50,50), U32(35,35,35));
            dl->AddRectFilled(ImVec2(sx+2, sy+2+i*2), ImVec2(sx+ww-2, sy+4+i*2), c);
        }
        dl->AddRect(ImVec2(sx, sy), ImVec2(sx+ww, sy+22), U32(30,30,30));
        dl->AddRect(ImVec2(sx+1, sy+1), ImVec2(sx+ww-1, sy+21), U32(0,0,0));
        bool confirm = w.doubleClick && (ImGui::GetTime() - w.lastClickTime < 4.0);
        const char* lbl = confirm ? "Confirm?" : w.name.c_str();
        ImU32 lc = confirm ? theme::accentU32() : U32(255,255,255);
        ImVec2 ts = ImGui::CalcTextSize(lbl);
        dl->AddText(ImVec2(sx+(ww-ts.x)/2, sy+4), lc, lbl);
        ImGui::SetCursorScreenPos(ImVec2(sx, sy));
        ImGui::InvisibleButton(("##btn"+w.name).c_str(), ImVec2(ww, 22));
        if (ImGui::IsItemClicked()) {
            if (w.doubleClick) {
                if (ImGui::GetTime() - w.lastClickTime < 4.0) {
                    w.lastClickTime = 0;
                    if (w.onButton) w.onButton(w);
                } else w.lastClickTime = (float)ImGui::GetTime();
            } else if (w.onButton) w.onButton(w);
        }
        break;
    }
    default: break;
    }
    ImGui::PopID();
}

float Menu::drawGroupBox(Group& g, float gx, float gy, float gw) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 org = ImGui::GetWindowPos();
    float sx = org.x + gx, sy = org.y + gy;

    float h = 30.f;
    auto& wlist = g.isMulti() ? g.subTabWidgets[g.activeSubTab] : g.widgets;
    for (auto& w : wlist) h += widgetHeight(w);
    h += 8.f;
    if (g.isMulti()) h += 24.f;

    dl->AddRectFilled(ImVec2(sx, sy), ImVec2(sx+gw, sy+h), theme::bg());
    dl->AddRect(ImVec2(sx, sy), ImVec2(sx+gw, sy+h), U32(0,0,0));
    dl->AddRect(ImVec2(sx+1, sy+1), ImVec2(sx+gw-1, sy+h-1), theme::border());
    dl->AddRectFilled(ImVec2(sx+2, sy+2), ImVec2(sx+gw-2, sy+3), theme::accentU32());
    dl->AddRectFilled(ImVec2(sx+2, sy+3), ImVec2(sx+gw-2, sy+4), theme::accentDarkU32());
    dl->AddRectFilled(ImVec2(sx+2, sy+4), ImVec2(sx+gw-2, sy+5), theme::border());
    dl->AddText(ImVec2(sx+6, sy+5), U32(255,255,255), g.name.c_str());

    float cy = gy + 28.f;
    if (g.isMulti()) {
        float stripY = sy + 28, tx = sx + 2;
        for (size_t i = 0; i < g.subTabNames.size(); i++) {
            ImVec2 ts = ImGui::CalcTextSize(g.subTabNames[i].c_str());
            float bw = ts.x + 12;
            ImU32 bg = (i == (size_t)g.activeSubTab) ? theme::accentU32() : U32(45,45,45);
            dl->AddRectFilled(ImVec2(tx, stripY), ImVec2(tx+bw, stripY+20), bg);
            dl->AddText(ImVec2(tx+6, stripY+3), U32(255,255,255), g.subTabNames[i].c_str());
            ImGui::SetCursorScreenPos(ImVec2(tx, stripY));
            ImGui::InvisibleButton(("##sub"+g.subTabNames[i]).c_str(), ImVec2(bw, 20));
            if (ImGui::IsItemClicked()) g.activeSubTab = (int)i;
            tx += bw + 4;
        }
        cy += 24.f;
    }
    for (auto& w : wlist) { drawWidget(w, gx + 8, cy, gw - 16); cy += widgetHeight(w); }
    return h;
}

void Menu::drawTabBar() {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 org = ImGui::GetWindowPos();
    float tabW = (width - 20) / (float)tabs.size();
    for (size_t i = 0; i < tabs.size(); i++) {
        float sx = org.x + 10 + i * tabW, sy = org.y + 27;
        ImU32 bg = (i == (size_t)activeTab) ? U32(50,50,50) : U32(30,30,30);
        dl->AddRectFilled(ImVec2(sx, sy), ImVec2(sx+tabW, sy+TAB_H), bg);
        dl->AddRect(ImVec2(sx, sy), ImVec2(sx+tabW, sy+TAB_H), theme::border());
        ImVec2 ts = ImGui::CalcTextSize(tabs[i].name.c_str());
        dl->AddText(ImVec2(sx+(tabW-ts.x)/2, sy+(TAB_H-ts.y)/2),
                    i == (size_t)activeTab ? U32(255,255,255) : U32(170,170,170),
                    tabs[i].name.c_str());
        ImGui::SetCursorScreenPos(ImVec2(sx, sy));
        ImGui::InvisibleButton(("##tab"+tabs[i].name).c_str(), ImVec2(tabW, TAB_H));
        if (ImGui::IsItemClicked()) activeTab = (int)i;
    }
}

void Menu::render() {
    if (!open && fadeAlpha <= 0.01f) return;
    if (fading) {
        float t = (float)ImGui::GetTime() - fadeStart;
        if (open) fadeAlpha = std::min(1.f, t * 3.f);
        else      fadeAlpha = std::max(0.f, 1.f - t * 3.f);
        if ((open && fadeAlpha >= 1.f) || (!open && fadeAlpha <= 0.f)) fading = false;
    } else fadeAlpha = open ? 1.f : 0.f;

    ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertU32ToFloat4(theme::bg()));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0,0,0,1));
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, fadeAlpha);
    ImGui::Begin("##bbmenu", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

    ImVec2 pos = ImGui::GetWindowPos();
    x = pos.x; y = pos.y;
    ImDrawList* dl = ImGui::GetWindowDrawList();

    dl->AddRect(ImVec2(pos.x, pos.y), ImVec2(pos.x+width, pos.y+height), U32(0,0,0));
    dl->AddRect(ImVec2(pos.x+1, pos.y+1), ImVec2(pos.x+width-1, pos.y+height-1), theme::border());
    dl->AddRectFilled(ImVec2(pos.x+2, pos.y+2), ImVec2(pos.x+width-2, pos.y+3), theme::accentU32());
    dl->AddRectFilled(ImVec2(pos.x+2, pos.y+3), ImVec2(pos.x+width-2, pos.y+4), theme::accentDarkU32());
    for (int i = 0; i < 20; i++) {
        ImU32 c = ColorRange((float)i, 0, 20, U32(50,50,50), U32(35,35,35));
        dl->AddRectFilled(ImVec2(pos.x+2, pos.y+5+i), ImVec2(pos.x+width-2, pos.y+6+i), c);
    }
    dl->AddText(ImVec2(pos.x+6, pos.y+6), U32(255,255,255), menuName.c_str());
    dl->AddRectFilled(ImVec2(pos.x+2, pos.y+25), ImVec2(pos.x+width-2, pos.y+height-2), theme::bg());
    dl->AddRect(ImVec2(pos.x+8, pos.y+22), ImVec2(pos.x+width-8, pos.y+height-8), U32(0,0,0));
    dl->AddRect(ImVec2(pos.x+9, pos.y+23), ImVec2(pos.x+width-9, pos.y+height-9), theme::border());
    dl->AddRectFilled(ImVec2(pos.x+10, pos.y+24), ImVec2(pos.x+width-10, pos.y+25), theme::accentU32());
    dl->AddRectFilled(ImVec2(pos.x+10, pos.y+25), ImVec2(pos.x+width-10, pos.y+26), theme::accentDarkU32());
    for (int i = 0; i < 15; i++) {
        ImU32 c = ColorRange((float)i, 0, 15, U32(50,50,50), U32(35,35,35));
        dl->AddRectFilled(ImVec2(pos.x+10, pos.y+27+i*2), ImVec2(pos.x+width-10, pos.y+29+i*2), c);
    }
    dl->AddRectFilled(ImVec2(pos.x+10, pos.y+57), ImVec2(pos.x+width-10, pos.y+height-10), theme::bg());

    drawTabBar();
    if (tabs.empty()) { ImGui::End(); ImGui::PopStyleVar(3); ImGui::PopStyleColor(2); return; }
    Tab& tab = tabs[activeTab];
    float yL = TOP_Y, yR = TOP_Y;
    for (auto& g : tab.groups) {
        bool isRight = (g.autoPos == "right");
        float gx = (g.width > 0) ? g.x : (isRight ? COL_RIGHT : COL_LEFT);
        float gy = (g.y > 0)     ? g.y : (isRight ? yR        : yL);
        float gw = (g.width > 0) ? g.width : COL_W;
        theme::accent = accent;
        theme::accentDark = accentDark;
        float usedH = drawGroupBox(g, gx, gy, gw);
        if (g.autofill) usedH = std::max(usedH, height - gy - 17.f);
        if (isRight) yR = gy + usedH + 6; else yL = gy + usedH + 6;
    }

    auto* acc = find("Settings", "Cheat Settings", "Menu Accent");
    if (acc && acc->boolValue) {
        accent = acc->color;
        accentDark = V4((int)(acc->color.x*255)-40, (int)(acc->color.y*255)-40, (int)(acc->color.z*255)-40);
    } else { accent = V4(127,72,163); accentDark = V4(87,32,123); }
    theme::accent = accent; theme::accentDark = accentDark;

    auto* cmn = find("Settings", "Cheat Settings", "Custom Menu Name");
    auto* mn  = find("Settings", "Cheat Settings", "MenuName");
    if (cmn && mn) menuName = cmn->boolValue ? mn->textValue : "Bitch Bot";
    auto* wm = find("Settings", "Cheat Settings", "Watermark");
    watermarkEnabled = wm && wm->boolValue;
    auto* kh = find("Settings", "Cheat Settings", "Keybind List");
    keybindHudEnabled = kh && kh->boolValue;

    ImGui::End();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
    drawNotifications();

    if (ImGui::IsKeyPressed(ImGuiKey_Delete, false)) {
        open = !open;
        fading = true;
        fadeStart = (float)ImGui::GetTime();
    }
}

void Menu::notify(const std::string& text) {
    Notification n; n.text = text; n.targetAlpha = 1.f;
    notifications.push_back(n);
    if (notifications.size() > 6) notifications.erase(notifications.begin());
}

void Menu::drawNotifications() {
    ImGuiIO& io = ImGui::GetIO();
    float baseY = 40.f;
    for (auto& n : notifications) {
        n.timeLeft -= io.DeltaTime;
        if (n.timeLeft < 0.5f) n.targetAlpha = 0.f;
        float speed = n.targetAlpha > n.alpha ? 10.f : 4.f;
        n.alpha += (n.targetAlpha - n.alpha) * io.DeltaTime * speed;
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 260, baseY));
        ImGui::SetNextWindowSize(ImVec2(240, 26));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, n.alpha);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertU32ToFloat4(theme::bg()));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0,0,0,0));
        ImGui::Begin(("##notif" + n.text).c_str(), nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoInputs);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetWindowPos();
        dl->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x+3, p.y+26), theme::accentU32());
        dl->AddText(ImVec2(p.x+8, p.y+6), U32(255,255,255), n.text.c_str());
        ImGui::End();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
        baseY += 30;
    }
    notifications.erase(std::remove_if(notifications.begin(), notifications.end(),
        [](const Notification& n){ return n.timeLeft <= 0.f && n.alpha < 0.01f; }),
        notifications.end());
}

void Menu::handleKeybinds() {
    for (auto& t : tabs)
        for (auto& g : t.groups) {
            auto proc = [&](Widget& w) {
                if (!w.hasKeybind || w.keyCode == ImGuiKey_None) return;
                bool wasActive = w.keybindActive;
                if (w.keyToggleMode == ToggleMode::Hold)
                    w.keybindActive = ImGui::IsKeyDown(w.keyCode);
                else if (w.keyToggleMode == ToggleMode::Toggle)
                    if (ImGui::IsKeyPressed(w.keyCode, false)) w.keybindActive = !w.keybindActive;
                else if (w.keyToggleMode == ToggleMode::HoldOff)
                    w.keybindActive = !ImGui::IsKeyDown(w.keyCode);
                else
                    w.keybindActive = true;
                if (wasActive != w.keybindActive && w.onChange) w.onChange(w);
            };
            for (auto& w : g.widgets) proc(w);
            for (auto& v : g.subTabWidgets) for (auto& w : v) proc(w);
        }
}

std::vector<Menu::ActiveKeybind> Menu::getActiveKeybinds() {
    std::vector<ActiveKeybind> out;
    for (auto& t : tabs)
        for (auto& g : t.groups) {
            auto proc = [&](Widget& w) {
                if (!w.hasKeybind || !w.boolValue) return;
                if (w.name == "Menu Key") return;
                if (w.keyToggleMode == ToggleMode::Always || w.keybindActive)
                    out.push_back({ w.name, keyName(w.keyCode) });
            };
            for (auto& w : g.widgets) proc(w);
            for (auto& v : g.subTabWidgets) for (auto& w : v) proc(w);
        }
    return out;
}

void Menu::tickCallbacks() {
    for (auto& t : tabs)
        for (auto& g : t.groups) {
            auto proc = [&](Widget& w) {
                if (!w.initialized) {
                    w.initialized = true;
                    w.lastBool = w.boolValue; w.lastFloat = w.floatValue;
                    w.lastText = w.textValue; w.lastInt = w.intValue;
                    return;
                }
                if (w.type == WidgetType::Toggle && w.lastBool != w.boolValue) { w.lastBool = w.boolValue; if (w.onChange) w.onChange(w); }
                if (w.type == WidgetType::Slider && w.lastFloat != w.floatValue) { w.lastFloat = w.floatValue; if (w.onChange) w.onChange(w); }
                if (w.type == WidgetType::Dropbox && w.lastInt != w.intValue) { w.lastInt = w.intValue; if (w.onChange) w.onChange(w); }
                if (w.type == WidgetType::Textbox && w.lastText != w.textValue) { w.lastText = w.textValue; if (w.onChange) w.onChange(w); }
            };
            for (auto& w : g.widgets) proc(w);
            for (auto& v : g.subTabWidgets) for (auto& w : v) proc(w);
        }
}

bool Menu::saveConfig(const std::string& name) {
    fs::create_directories("bitchbot/" + game);
    std::ofstream f("bitchbot/" + game + "/" + name + ".bb");
    if (!f) return false;
    f << "BitchBot v2\n";
    for (auto& t : tabs)
        for (auto& g : t.groups) {
            auto proc = [&](Widget& w) {
                f << t.name << "|" << g.name << "|" << w.name << "|";
                switch (w.type) {
                    case WidgetType::Toggle:  f << (w.boolValue ? 1 : 0); break;
                    case WidgetType::Slider:  f << w.floatValue; break;
                    case WidgetType::Dropbox: f << w.intValue; break;
                    case WidgetType::Combobox:
                        for (auto& [n, v] : w.multiOptions) f << (v ? 1 : 0);
                        break;
                    case WidgetType::Textbox: f << w.textValue; break;
                    default: break;
                }
                if (w.hasKeybind) f << "|kb:" << (int)w.keyCode << "|mode:" << (int)w.keyToggleMode;
                if (w.hasColor) f << "|col:" << (int)(w.color.x*255) << "," << (int)(w.color.y*255) << "," << (int)(w.color.z*255);
                f << "\n";
            };
            for (auto& w : g.widgets) proc(w);
            for (auto& v : g.subTabWidgets) for (auto& w : v) proc(w);
        }
    return true;
}

bool Menu::loadConfig(const std::string& name) {
    std::ifstream f("bitchbot/" + game + "/" + name + ".bb");
    if (!f) return false;
    std::string line; std::getline(f, line);
    while (std::getline(f, line)) {
        std::vector<std::string> parts;
        std::stringstream ss(line); std::string tok;
        while (std::getline(ss, tok, '|')) parts.push_back(tok);
        if (parts.size() < 4) continue;
        Widget* w = find(parts[0], parts[1], parts[2]);
        if (!w) continue;
        if (w->type == WidgetType::Toggle) w->boolValue = (parts[3] == "1");
        else if (w->type == WidgetType::Slider) w->floatValue = std::stof(parts[3]);
        else if (w->type == WidgetType::Dropbox) w->intValue = std::stoi(parts[3]);
        else if (w->type == WidgetType::Textbox) w->textValue = parts[3];
        for (size_t i = 4; i < parts.size(); i++) {
            if (parts[i].rfind("kb:", 0) == 0) w->keyCode = (ImGuiKey)std::stoi(parts[i].substr(3));
            else if (parts[i].rfind("mode:", 0) == 0) w->keyToggleMode = (ToggleMode)std::stoi(parts[i].substr(5));
        }
    }
    return true;
}

std::vector<std::string> Menu::listConfigs() {
    std::vector<std::string> out;
    std::error_code ec;
    for (auto& p : fs::directory_iterator("bitchbot/" + game, ec))
        if (p.path().extension() == ".bb")
            out.push_back(p.path().stem().string());
    if (out.empty()) out.push_back("Default");
    return out;
}