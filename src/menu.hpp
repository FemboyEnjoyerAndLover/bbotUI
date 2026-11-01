#pragma once
#include <string>
#include <vector>
#include <functional>
#include "imgui.h"
#include "theme.hpp" 

enum class WidgetType { Toggle, Slider, Dropbox, Combobox, Textbox, Button, List };
enum class ToggleMode { Hold = 1, Toggle = 2, HoldOff = 3, Always = 4 };

struct Widget {
    WidgetType type = WidgetType::Toggle;
    std::string name;
    bool boolValue = false, unsafe = false;
    float floatValue = 0, minValue = 0, maxValue = 100;
    float step = 1.f, decimal = 0.f;
    std::string suffix;
    int intValue = 0;
    std::vector<std::string> options;
    std::vector<std::pair<std::string, bool>> multiOptions;
    std::string textValue;
    bool isFile = false;
    bool doubleClick = false;
    double lastClickTime = 0.0;
    bool hasKeybind = false;
    ImGuiKey keyCode = ImGuiKey_None;
    ToggleMode keyToggleMode = ToggleMode::Toggle;
    bool keybindActive = false, listeningForKey = false;
    bool hasColor = false, hasDoubleColor = false;
    ImVec4 color = ImVec4(1,1,1,1), color2 = ImVec4(1,1,1,1);
    std::string tooltip;
    std::function<void(Widget&)> onChange;
    std::function<void(Widget&)> onButton;
    bool lastBool = false;
    std::string lastText;
    float lastFloat = 0;
    int lastInt = 0;
    bool initialized = false;
};

struct Group {
    std::string name, autoPos;
    float x = 0, y = 0, width = 0, height = 0;
    bool autofill = false;
    std::vector<std::string> subTabNames;
    std::vector<std::vector<Widget>> subTabWidgets;
    int activeSubTab = 0;
    std::vector<Widget> widgets;
    bool isMulti() const { return !subTabNames.empty(); }
};

struct Tab {
    std::string name;
    std::vector<Group> groups;
};

struct Notification {
    std::string text;
    float timeLeft = 4.f;
    float alpha = 0.f;
    float targetAlpha = 0.f;
};

class Menu {
public:
    void buildDemo();
    void render();
    Widget* find(const std::string& tab, const std::string& group, const std::string& widget);
    void notify(const std::string& text);
    void handleKeybinds();
    void tickCallbacks();
    bool saveConfig(const std::string& name);
    bool loadConfig(const std::string& name);
    std::vector<std::string> listConfigs();
    void renderPreview(ImDrawList* dl, ImVec2 origin, float scale);

    struct ActiveKeybind { std::string name, key; };
    std::vector<ActiveKeybind> getActiveKeybinds();

    std::vector<Tab> tabs;
    int   activeTab = 0;
    bool  open      = true;
    bool  fading    = false;
    float fadeStart = 0.f;
    float fadeAlpha = 1.f;
    float x = 50, y = 50;
    float width = 500, height = 600;
    ImVec4 accent     = V4(127, 72, 163);
    ImVec4 accentDark = V4(87, 32, 123);
    std::string menuName = "Bitch Bot";
    bool customMenuName = false;
    std::string game = "uni";
    std::string currentConfigName = "Default";
    bool watermarkEnabled = true;
    bool keybindHudEnabled = true;
    std::vector<Notification> notifications;
    float fps = 0.f;
    float ping = 0.f;

private:
    float drawGroupBox(Group& g, float gx, float gy, float gw);
    void  drawWidget(Widget& w, float wx, float wy, float ww);
    void  drawTabBar();
    void  drawNotifications();
};