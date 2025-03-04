#include "renderer.h"

void Renderer::Setup()
{
    ofSetFrameRate(60);

    ofSetWindowShape(1600, 900);
}

void Renderer::Draw()
{
    ofClear(0, 0, 0, 255);

    this->scenes.DrawSelectedScene();
}
