//
// Created by thierrycd on 2025-01-14.
//

#include "GLFWContext.h"

#include <iostream>


void error_callback(const int error, const char* description)
{
    std::cerr << "Error {" << error << "}: " << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    std::cout << "Key callback: " << key << "," << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}


GLFWContext::GLFWContext() {
    std::cout << "Initializing GLFW" << std::endl;
    if (!glfwInit())
    {
        std::cerr << "Could not init glfw" << std::endl;
        exit(1);
    }

    glfwSetErrorCallback(error_callback);
}

GLFWContext::~GLFWContext() {
    std::cout << "Terminating GLFW" << std::endl;

    if (this->window) {
        glfwDestroyWindow(this->window);
    }

    glfwTerminate();
}

void GLFWContext::init_window(const int width, const int height, const char* title)
{
    if (this->window)
    {
        std::cout << "Destroying previous window" << std::endl;
        glfwDestroyWindow(this->window);
        this->window = nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Could not create window" << std::endl;
        this->~GLFWContext();
        exit(1);
    }


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

bool GLFWContext::window_should_close() const
{
    return glfwWindowShouldClose(this->window);
}

void GLFWContext::init_viewport_from_window() const
{
    int width, height;
    glfwGetFramebufferSize(this->window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(this->window,[](GLFWwindow*, const int width, const int height){std::cout << "Framebuffer size changed: " << width << "," << height << std::endl;});
}
