cmake_minimum_required(VERSION 3.16)
project(bbui CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui)

add_executable(bbui
    src/main.cpp
    src/menu.cpp
    src/menu_renderpreview.cpp
    src/theme.cpp
    src/extras.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(bbui PRIVATE src ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(bbui PRIVATE glfw OpenGL::GL)
if (MSVC)
    target_compile_options(bbui PRIVATE /utf-8)
endif()