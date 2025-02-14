#include "application.h"

#include "renderer/scenes/ImportImportExportScene.h"

void Application::setup()
{
    ofSetWindowTitle("Bonjour Monde");

    ofLog() << "<app::setup>";

    gui.setup();

    renderer.Setup();

    this->importExportImageScene = std::make_shared<ImportImportExportScene>();
    this->importExportImageScene->setup();
    renderer.scenes.AddScene(this->importExportImageScene);

    this->rotatingCubeScene = std::make_shared<RotatingCubeScene>();
    this->rotatingCubeScene->setup();
    renderer.scenes.AddScene(this->rotatingCubeScene);
}

void Application::draw()
{
    gui.begin();

    renderer.Draw();

    ImGui::Begin("Scene selection");
    for (const auto& scene: renderer.scenes.GetScenes())
    {
        SceneId id = scene.first;
        std::string name = scene.second->GetName();
        if (ImGui::Button(name.c_str())) { this->SelectScene(id); }
    }
    ImGui::End();

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
