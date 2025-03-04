#pragma once

#include "Gui.h"
#include "ofMain.h"
#include "scenes/Scene.h"
#include "scenes/Scenes.h"

class Renderer
{
public:
    void Setup();
    void Draw();
    void Update();
    Scenes scenes{};
};
