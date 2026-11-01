#pragma once
#include "imgui.h"
#include "menu.hpp"

class Watermark   { public: void render(Menu& menu); };
class KeybindHUD  { public: void render(Menu& menu); };
class Preview     { public: void render(Menu& menu); bool visible = true; };