#pragma once

#include "opengl/context/GLFWContext.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imfilebrowser.h>
#include <imgui.h>

class IMGUIContext
{
public:
    explicit IMGUIContext(GLFWContext* glfwContext);
    ~IMGUIContext();

    ImGuiIO& GetIO() { return ImGui::GetIO(); }

    bool NewFrame();
    void Render();

private:
    GLFWContext* glfwContext;
};
