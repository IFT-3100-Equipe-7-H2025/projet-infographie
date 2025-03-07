#pragma once

#include "Gui.h"
#include "ofMain.h"
#include "scenes/Scenes.h"

class Renderer
{
public:
    void Setup();
    void Draw();
    void Update();
    Scenes scenes{};

    int targetFPS = 60;
    void SetTargetFPS(int targetFPS)
    {
        this->targetFPS = targetFPS;
        ofSetFrameRate(targetFPS);
    }

    bool vsync = true;
    void SetVsync(bool vsync)
    {
        this->vsync = vsync;
        ofSetVerticalSync(vsync);
    }

    bool showFPS = true;
    ofTrueTypeFont font;
};
