#include "Scenes.h"

void Scenes::AddScene(const std::shared_ptr<Scene>& scene)
{
    const auto id = scene->getId();
    scenes[id] = scene;
    if (scenes.size() == 1) { SelectScene(id); }
}

void Scenes::SelectScene(const SceneId id)
{
    if (scenes.find(id) == scenes.end()) { throw std::runtime_error("Scene with id " + std::to_string(id) + " does not exist"); }

    this->selectedScene = id;
}

void Scenes::DrawSelectedScene()
{
    if (scenes.find(selectedScene) == scenes.end()) { throw std::runtime_error("Selected scene does not exist"); }
    scenes[selectedScene]->draw();
}

const std::shared_ptr<Scene>& Scenes::GetScene(const SceneId id) { return scenes[id]; }
const std::unordered_map<SceneId, std::shared_ptr<Scene>>& Scenes::GetScenes() { return scenes; }

std::shared_ptr<Scene> Scenes::GetSelectedScene() const {
    auto it = scenes.find(selectedScene);
    if (it == scenes.end()) {
        throw std::runtime_error("Selected scene does not exist");
    }
    return it->second;
}