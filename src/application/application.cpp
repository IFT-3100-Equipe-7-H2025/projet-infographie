#include "application.h"
#include "ofAppRunner.h"

#include <Macros.h>

void Application::setup()
{
    ofSetWindowTitle("Bonjour Monde");

    ofLog() << "<app::setup>";

    gui.setup();

    renderer.Setup();

    this->scene3D = std::make_shared<Scene3D>();
    this->scene3D->setup();
    renderer.scenes.AddScene(this->scene3D);

    this->importExportImageScene = std::make_shared<ImportImageScene>();
    this->importExportImageScene->setup();
    renderer.scenes.AddScene(this->importExportImageScene);

    this->rotatingCubeScene = std::make_shared<RotatingCubeScene>();
    this->rotatingCubeScene->setup();
    renderer.scenes.AddScene(this->rotatingCubeScene);
}

void Application::draw()
{

    gui.begin();

    this->ShowMainMenuBar();

    renderer.Draw();

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
