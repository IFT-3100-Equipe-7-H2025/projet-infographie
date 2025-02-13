#pragma once


#include "debug-trap.h"
#include <iostream>
#include <GL/glew.h>

// Macro to assert OpenGL calls, uses psnip_trap() to put a breakpoint if the call fails
#define GLAssert(x) \
    if (!(x)) psnip_trap();

inline static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, const int line)
{
    if (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (0x" << std::hex << error << "): " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}


#define GLCall(x)   \
    GLClearError(); \
    x;              \
    GLAssert(GLLogCall(#x, __FILE__, __LINE__));
