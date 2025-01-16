#include "context/GLFWContext.h"
#include "shader.h"

constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;

int main()
{
    const GLFWContext context = GLFWContext(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", nullptr, nullptr);

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), shader::positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);

    const unsigned int shader = shader::CreateShader(shader::vertexShader, shader::fragmentShader);
    glUseProgram(shader);

    GLFWwindow* window = context.GetWindow();
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    return 0;
}
