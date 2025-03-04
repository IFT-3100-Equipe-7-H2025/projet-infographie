#include "application.h"
#include "GeometryScene.h"
#include "imgui.h"

#include <Macros.h>

void Application::setup()
{
    ofSetWindowTitle("Projet Infographie - 3D Editor");

    ofHideCursor();
    cursorManager.setup();

    ofLog() << "<app::setup>";

    gui.setup(nullptr, true, ImGuiConfigFlags_DockingEnable, false);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;


    renderer.Setup();

    this->geometryScene = std::make_shared<GeometryScene>();
    this->geometryScene->setup();
    renderer.scenes.AddScene(this->geometryScene);

    this->importExportImageScene = std::make_shared<ImportImageScene>();
    this->importExportImageScene->setup();
    renderer.scenes.AddScene(this->importExportImageScene);

    this->rotatingCubeScene = std::make_shared<RotatingCubeScene>();
    this->rotatingCubeScene->setup();
    renderer.scenes.AddScene(this->rotatingCubeScene);

    this->primitiveScene = std::make_shared<PrimitiveScene>();
    this->primitiveScene->setup();
    renderer.scenes.AddScene(this->primitiveScene);

    
}

void Application::draw()
{
    gui.begin();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    this->ShowMainMenuBar();
    renderer.Draw();

    if (ImGui::GetIO().WantCaptureMouse)
    {
        cursorManager.setState(CursorState::HoverButton);
    }
    else
    {
        auto currentScene = renderer.scenes.GetSelectedScene();
        if (currentScene)
        {
            auto primitiveScene = dynamic_cast<PrimitiveScene*>(currentScene.get());
            if (primitiveScene)
            {
                if (primitiveScene->getIsWaitingForSecondClick())
                {
                    cursorManager.setState(CursorState::WaitingSecondClick);
                }
                else if (primitiveScene->getIsAdding())
                {
                    if (primitiveScene->getSelectedType() == PrimitiveType::Point)
                    {
                        cursorManager.setState(CursorState::DrawPoint);
                    }
                    else
                    {
                        cursorManager.setState(CursorState::DrawPrimitive);
                    }
                }
                else
                {
                    cursorManager.setState(CursorState::Default);
                }
            }
            else
            {
                cursorManager.setState(CursorState::Default);
            }
        }
    }

    cursorManager.drawForeground();

    gui.end();
}

void Application::keyReleased(int key)
{
    ofLog() << "<app::keyReleased: " << key << ">";
}

void Application::mouseReleased(int x, int y, int button)
{
    ofLog() << "<app::mouse released at: (" << x << ", " << y << ")>";
}

void Application::mousePressed(int x, int y, int button)
{
    try
    {
        auto selectedScene = renderer.scenes.GetSelectedScene();
        if (selectedScene)
        {
            selectedScene->mousePressed(x, y, button);
        }
    } catch (std::exception& e)
    {
        ofLog() << "Aucune scène sélectionnée pour mousePressed : " << e.what();
    }
}

void Application::exit()
{
    ofLog() << "<app::exit>";
}

void Application::SelectScene(SceneId id) { renderer.scenes.SelectScene(id); }

void Application::ShowMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Scene selection"))
        {
            for (const auto& scene: renderer.scenes.GetScenes())
            {
                SceneId id = scene.first;
                std::string name = scene.second->GetName();
                if (ImGui::MenuItem(name.c_str()))
                {
                    this->SelectScene(id);
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::Button("Export screen as image"))
        {
            this->ExportImageButtonPressed();
        }

        ImGui::EndMainMenuBar();
    }
}

void Application::ExportImage(const std::string& path)
{
    GLCall(glReadBuffer(GL_FRONT));
    ofSaveScreen(path);
}

void Application::ExportImageButtonPressed()
{
    // there is a bug in the current version of openFrameworks that causes the parameters to not be used
    if (const ofFileDialogResult result = ofSystemSaveDialog("test.png", "Export image"); result.bSuccess)
    {
        ofLog() << "Successfully exported image: " << result.filePath;
        this->ExportImage(result.filePath);
    }
}
