//
// Created by thierrycd on 2025-01-16.
//
#include "GLFWContext.h"
#include "GLFW/glfw3.h"

#include <iostream>

void error_callback(const int error, const char* description)
{
    std::cerr << "GLFW Error {" << error << "}: " << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    const char* key_name = glfwGetKeyName(key, scancode);

    std::cout << "GLFW Key callback: " << key << ", "
              << (key == GLFW_KEY_ESCAPE ? "ESC" : key_name) << std::endl;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

GLFWContext::GLFWContext(const int width, const int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
    std::cout << "Initializing GLFW" << std::endl;
    if (!glfwInit())
    {
        std::cerr << "Could not init glfw" << std::endl;
        exit(1);
    }

    GLFWwindow* window = glfwCreateWindow(width, height, title, monitor, share);
    if (!window)
    {
        std::cerr << "Could not create window" << std::endl;
        this->~GLFWContext();
        exit(1);
    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSetWindowCloseCallback(window, [](GLFWwindow*) {
        std::cout << "Window closed" << std::endl;
    });
    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    if (const GLenum err = glewInit(); GLEW_OK != err)
    {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        this->~GLFWContext();
        exit(1);
    }

    glfwSwapInterval(1);

    this->window = window;
}

GLFWContext::~GLFWContext()
{
    std::cout << "Terminating GLFW" << std::endl;

    glfwDestroyWindow(this->window);
    glfwTerminate();
}
