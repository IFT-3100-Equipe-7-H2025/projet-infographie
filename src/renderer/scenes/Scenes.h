#pragma once
#include "Scene.h"

#include <memory>
#include <stdexcept>
#include <unordered_map>

class Scenes
{
public:
    void AddScene(const std::shared_ptr<Scene>& scene);

    void SelectScene(SceneId id);
    void DrawSelectedScene();

    [[nodiscard]] const std::shared_ptr<Scene>& GetScene(SceneId id);
    [[nodiscard]] const std::unordered_map<SceneId, std::shared_ptr<Scene>>& GetScenes();

    [[nodiscard]] std::shared_ptr<Scene> GetSelectedScene() const;

private:
    SceneId selectedScene = -1;
    std::unordered_map<SceneId, std::shared_ptr<Scene>> scenes{};
};
