#include "IMGUIContext.h"

IMGUIContext::IMGUIContext(GLFWContext* glfwContext)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(glfwContext->GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    this->glfwContext = glfwContext;
}

IMGUIContext::~IMGUIContext()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool IMGUIContext::NewFrame()
{
    if (this->glfwContext->GetWindowAttrib(GLFW_FOCUSED) == GLFW_FALSE)
    {
        ImGui_ImplGlfw_Sleep(10);
        return false;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    return true;
}

void IMGUIContext::Render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
