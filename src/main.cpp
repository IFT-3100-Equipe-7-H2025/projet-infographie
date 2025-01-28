// clang-format off
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imfilebrowser.h>
// clang-format on

#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "opengl/opengl_include.h"


#include "opengl/Macros.h"
#include "opengl/context/GLFWContext.h"

#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "opengl/rendering/IndexBuffer.h"
#include "opengl/rendering/Renderer.h"
#include "opengl/rendering/Shader.h"
#include "opengl/rendering/Texture.h"
#include "opengl/rendering/VertexArray.h"
#include "opengl/rendering/VertexArrayObject.h"
#include "opengl/rendering/VertexBuffer.h"
#include "opengl/rendering/VertexBufferLayout.h"
#include <cmath>
#include <cstdlib>


constexpr float image_size = 300.0f;

constexpr float positions[] = {
        // Front face
        0.0f, 0.0f, 0.0f, 0.0f,
        image_size, 0.0f, 1.0f, 0.0f,
        image_size, image_size, 1.0f, 1.0f,
        0, image_size, 0.0f, 1.0f};


constexpr unsigned int indices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0};

int WINDOW_WIDTH = 900;
int WINDOW_HEIGHT = 900;

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

    glClear(GL_COLOR_BUFFER_BIT);

    VertexArrayObject vao = VertexArrayObject();

    VertexArray va = VertexArray();
    VertexBuffer vb = VertexBuffer(reinterpret_cast<const void*>(positions), sizeof(positions));

    VertexBufferLayout layout = VertexBufferLayout();
    layout.Push<float>(2);
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    IndexBuffer ib = IndexBuffer(indices, sizeof(indices) / sizeof(unsigned int));

    Shader shader = Shader("res/shaders/Texture.glsl");
    shader.Bind();

    GLFWwindow* window = context.GetWindow();

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    Renderer renderer = Renderer();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");


    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGui::FileBrowser fileDialog;

    // (optional) set browser properties
    fileDialog.SetTitle("title");
    fileDialog.SetTypeFilters({".png", ".jpg"});

    std::string filename = "";
    Texture texture;

    glm::mat4 proj = glm::ortho(0.0f,
                                static_cast<float>(WINDOW_WIDTH),
                                0.0f,
                                static_cast<float>(WINDOW_HEIGHT),
                                -1.0f,
                                1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(10, 10, 0));
    glm::mat4 mvp;
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");

            ImGui::Text("This is some useful text.");
            if (ImGui::Button("Open file dialog"))
            {
                fileDialog.Open();
            }

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();

            fileDialog.Display();

            if (fileDialog.HasSelected())
            {
                std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
                texture.Load(fileDialog.GetSelected().string());
                fileDialog.ClearSelected();
            }
        }

        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        if (texture.IsLoaded())
        {
            shader.Bind();
            texture.Bind(0);
            shader.SetUniform1i("uTexture", 0);
            mvp = proj * view * glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0));
            shader.SetUniformMatrix4f("mvp", mvp);
            renderer.Draw(va, ib, shader);
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
