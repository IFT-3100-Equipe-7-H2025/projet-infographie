// IFT3100H25_BonjourMonde/renderer.h
// Classe responsable du rendu de l'application.

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

    Scenes scenes{};
};
