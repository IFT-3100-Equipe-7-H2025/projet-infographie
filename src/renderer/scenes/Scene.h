#pragma once
#include "ofBaseApp.h"

#include <string>


using SceneId = int;

class Scene : public ofBaseApp
{
public:
    Scene() : id(nextId++) {}

    [[nodiscard]] SceneId getId() const { return id; }
    void                  setShouldDrawScene(const bool shouldDraw) { this->shouldDraw = shouldDraw; }
    [[nodiscard]] bool    shouldDrawScene() const { return shouldDraw; }

    virtual std::string GetName() = 0;

private:
    SceneId        id;

    bool           shouldDraw = false;
    static SceneId nextId;
};

SceneId Scene::nextId = 0;
