//
// Created by thierrycd on 2025-01-14.
//

#include "GLFWContext.h"
#include <iostream>
#include <GLFW/glfw3.h>


GLFWContext::GLFWContext() {
    if (!glfwInit())
    {
        std::cerr << "Could not init glfw" << std::endl;
        exit(1);
    }
}

GLFWContext::~GLFWContext() {
    glfwTerminate();
}