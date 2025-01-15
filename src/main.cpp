#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glfw/GLFWContext.h"
#include "linmath.h"

typedef struct Vertex
{
    vec2 pos;
    vec3 col;
} Vertex;

static const Vertex vertices[3] =
{
    { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } },
    { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } },
    { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } }
};

static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"in vec3 vCol;\n"
"in vec2 vPos;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
"in vec3 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    fragment = vec4(color, 1.0);\n"
"}\n";

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

int main(void)
{
    GLFWContext context = GLFWContext();
    context.init_window(WIDTH, HEIGHT, "Hello World");
    context.init_viewport_from_window();



    while (!context.window_should_close())
    {
        glClear(GL_COLOR_BUFFER_BIT);

        context.swap_buffers();
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}