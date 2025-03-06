#include "renderer.h"
#include "ofGraphics.h"
#include "ofLog.h"

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Renderer::Setup()
{
    ofSetFrameRate(60);

    ofSetWindowShape(1600, 900);

    ofEnableDepthTest();
    ofEnableLighting();

    //ofSetLogLevel(ofLogLevel::OF_LOG_VERBOSE);
}

void Renderer::Update()
{
    this->scenes.UpdateSelectedScene();
}

void Renderer::Draw()
{
    ofClear(0, 0, 0, 255);

    this->scenes.DrawSelectedScene();
}
