#include "opengl/Macros.h"
#include "opengl/context/GLFWContext.h"

#include "opengl/opengl_include.h"
#include "opengl/rendering/Shader.h"
#include "shader.h"
#include <cstdlib>

constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;

int main()
{
    const GLFWContext context = GLFWContext(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", nullptr, nullptr);

    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), shader::positions, GL_STATIC_DRAW));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr));

    Shader shader = Shader("res/shaders/Basic.shader");
    shader.Bind();

    GLFWwindow* window = context.GetWindow();
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        GLCall(glDrawArrays(GL_TRIANGLES, 0, 3));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
