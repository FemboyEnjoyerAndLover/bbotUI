#include <cstdio>
#include <cstdlib>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "menu.hpp"
#include "theme.hpp"
#include "extras.hpp"

int main(int, char**) {
    if (!glfwInit()) { fprintf(stderr, "glfwInit failed\n"); return 1; }
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(1280, 800, "Bitch Bot — C++", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    theme::applyStyle();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    Menu menu;
    menu.buildDemo();
    menu.notify("Done loading the unicheat. (42 ms)");
    menu.notify("Press DELETE to open and close the menu!");

    Watermark  watermark;
    KeybindHUD keybindHud;
    Preview    preview;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        menu.handleKeybinds();
        menu.tickCallbacks();
        menu.ping = 12.f + (float)(rand() % 15);

        menu.render();
        watermark.render(menu);
        keybindHud.render(menu);
        preview.render(menu);

        if (ImGui::IsKeyPressed(ImGuiKey_Escape, false))
            glfwSetWindowShouldClose(window, true);

        ImGui::Render();
        int w, h; glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.08f, 0.08f, 0.08f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}