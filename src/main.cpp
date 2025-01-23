#include "opengl/opengl_include.h"


#include "opengl/Macros.h"
#include "opengl/context/GLFWContext.h"

#include "opengl/rendering/IndexBuffer.h"
#include "opengl/rendering/Shader.h"
#include "opengl/rendering/VertexArray.h"
#include "opengl/rendering/VertexArrayObject.h"
#include "opengl/rendering/VertexBuffer.h"
#include "opengl/rendering/VertexBufferLayout.h"
#include <cmath>
#include <cstdlib>


constexpr float positions[] = {
        -0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f, 0.5f,
        -0.5f, 0.5f};

constexpr unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0};

constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;

int main()
{
    const GLFWContext context = GLFWContext(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", nullptr, nullptr);

    VertexArrayObject vao = VertexArrayObject();

    VertexArray va;
    VertexBuffer vb = VertexBuffer(reinterpret_cast<const void*>(positions), sizeof(positions));

    VertexBufferLayout layout = VertexBufferLayout();
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    IndexBuffer ib(indices, sizeof(indices) / sizeof(unsigned int));

    Shader shader = Shader("res/shaders/Basic.shader");

    float r = 1.0f;
    float g = 0.5f;
    float b = 1.0f;
    float a = 1.0f;

    GLFWwindow* window = context.GetWindow();
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        vao.Bind();
        va.Bind();
        vb.Bind();
        ib.Bind();
        shader.Bind();

        shader.SetUniform4f("ourColor", r, g, b, a);

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        r = static_cast<float>(std::sin(glfwGetTime())) * 0.5f + 0.5f;
        g = static_cast<float>(std::cos(glfwGetTime())) * 0.5f + 0.5f;
        b = static_cast<float>(std::sin(glfwGetTime())) * 0.5f + 0.5f;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
