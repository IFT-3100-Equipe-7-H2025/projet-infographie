
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "opengl/opengl_include.h"


#include "opengl/Macros.h"
#include "opengl/context/GLFWContext.h"

#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "opengl/rendering/IndexBuffer.h"
#include "opengl/rendering/Shader.h"
#include "opengl/rendering/VertexArray.h"
#include "opengl/rendering/VertexArrayObject.h"
#include "opengl/rendering/VertexBuffer.h"
#include "opengl/rendering/VertexBufferLayout.h"
#include <cmath>
#include <cstdlib>


// constexpr float positions[] = {
//         -0.5f, -0.5f,
//         0.5f, -0.5f,
//         0.5f, 0.5f,
//         -0.5f, 0.5f};

// constexpr unsigned int indices[] = {
//         0, 1, 2,
//         2, 3, 0};

constexpr float positions[] = {
        // Front face
        -0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        // Back face
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        // Left face
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        // Right face
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        // Top face
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        -0.5f,
        // Bottom face
        -0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
};

constexpr unsigned int indices[] = {
        // Front face
        0,
        1,
        2,
        2,
        3,
        0,
        // Back face
        4,
        5,
        6,
        6,
        7,
        4,
        // Left face
        8,
        9,
        10,
        10,
        11,
        8,
        // Right face
        12,
        13,
        14,
        14,
        15,
        12,
        // Top face
        16,
        17,
        18,
        18,
        19,
        16,
        // Bottom face
        20,
        21,
        22,
        22,
        23,
        20,
};

int WINDOW_WIDTH = 640;
int WINDOW_HEIGHT = 480;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
}

int main()
{
    const GLFWContext context = GLFWContext(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", nullptr, nullptr);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    VertexArrayObject vao = VertexArrayObject();

    VertexArray va = VertexArray();
    VertexBuffer vb = VertexBuffer(reinterpret_cast<const void*>(positions), sizeof(positions));

    VertexBufferLayout layout = VertexBufferLayout();
    layout.Push<float>(3);
    va.AddBuffer(vb, layout);

    IndexBuffer ib = IndexBuffer(indices, sizeof(indices) / sizeof(unsigned int));

    Shader shader = Shader("res/shaders/Basic.shader");

    float r = 1.0f;
    float g = 0.5f;
    float b = 1.0f;
    float a = 1.0f;

    GLFWwindow* window = context.GetWindow();
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        vao.Bind();
        va.Bind();
        vb.Bind();
        ib.Bind();
        shader.Bind();

        glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

        shader.SetUniform4f("ourColor", r, g, b, a);
        shader.SetUniform3f("lightPos", lightPos.x, lightPos.y, lightPos.z);

        float fov = std::sin(glfwGetTime() + 1231239) * 40.0f;

        glm::vec3 camPos = glm::vec3(0.0, 0.0, -20.0);
        glm::vec3 lookPos = glm::vec3(0.0, 0.0, 10.0);

        glm::mat4 view = glm::lookAt(camPos, lookPos, glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 projection = glm::perspective(glm::radians(fov), static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 0.1f, 100.0f);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(fmod(static_cast<float>(std::sin(glfwGetTime())), 2) - 0.5f, fmod(static_cast<float>(std::cos(glfwGetTime())), 2) - 0.5f, fmod(static_cast<float>(std::sin(glfwGetTime()) + std::cos(glfwGetTime())), 2) - 0.5f));
        model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(1.0f, 1.0f, 1.0f));
        shader.SetUniformMatrix4f("mvp", projection * view * model);

        GLCall(glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, nullptr));

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


        r = static_cast<float>(std::sin(glfwGetTime())) * 0.5f + 0.5f;
        g = static_cast<float>(std::cos(glfwGetTime())) * 0.5f + 0.5f;
        b = static_cast<float>(std::sin(glfwGetTime())) * 0.5f + 0.5f;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
