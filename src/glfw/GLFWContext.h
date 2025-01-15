//
// Created by thierrycd on 2025-01-14.
//

#pragma once
#include <GL/glew.h>
#include "GLFW/glfw3.h"

class GLFWContext {
public:
    GLFWContext();
    ~GLFWContext();

    void init_window(int width, int height, const char* title);
    [[nodiscard]] bool window_should_close() const;

    void init_viewport_from_window() const;

    void swap_buffers() const
    {
        glfwSwapBuffers(this->window);
    }

private:
    GLFWwindow* window = nullptr;
};
