#pragma once

#include "opengl/opengl_include.h"

#include "debug-trap.h"
#include <iostream>

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
