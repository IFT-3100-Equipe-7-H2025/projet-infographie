//
// Created by thierrycd on 2025-01-16.
//

#pragma once
#include "opengl/opengl_include.h"

class GLFWContext
{
public:
    GLFWContext(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
    ~GLFWContext();

    [[nodiscard]] GLFWwindow* GetWindow() const { return window; }
    [[nodiscard]] int GetWindowAttrib(int attrib) const { return glfwGetWindowAttrib(window, attrib); }

private:
    GLFWwindow* window;
};
