# Bitch Bot C++ Port

## Build
1. Drop the imgui repo into `third_party/imgui` (branch: docking or master).
2. Install glfw3: `vcpkg install glfw3` (or your system package manager).
3. `cmake -B build -S . && cmake --build build --config Release`
4. Run `build/bbui`

## Keys
- `DELETE` — toggle the menu
- Sliders accept Shift+click for fine-step
- Buttons marked `double-click` need 2 clicks within 4s
- Configs save to `./bitchbot/uni/<name>.bb`

## Notes
This is a Windows/Linux/macOS desktop app. It is NOT a Roblox script;
the original Lua used executor `Drawing` + `UserInputService` which are
not available outside the game client. All visuals are recreated using
ImGui drawing primitives.