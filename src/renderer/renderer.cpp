#include "renderer.h"
#include "ofAppRunner.h"
#include "ofGraphics.h"
#include "ofLog.h"

constexpr unsigned int FPS_FONT_SIZE = 14;

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Renderer::Setup()
{
    ofSetFrameRate(targetFPS);
    ofSetVerticalSync(vsync);

    ofSetWindowShape(1600, 900);

    ofEnableDepthTest();
    ofEnableLighting();
    ofEnableAlphaBlending();

    //ofSetLogLevel(ofLogLevel::OF_LOG_VERBOSE);

    font.load("fonts/JetBrainsMono-Regular.ttf", FPS_FONT_SIZE, true, true);
}

void Renderer::Update()
{
    this->scenes.UpdateSelectedScene();
}

void Renderer::Draw()
{
    ofClear(0, 0, 0, 255);

    this->scenes.DrawSelectedScene();

    if (this->showFPS)
    {
        std::string fps = std::format("FPS: {:.4f}", ofGetFrameRate());
        ofPushStyle();
        ofSetColor(0, 255, 0);
        font.drawString(fps, static_cast<float>(ofGetWidth()) - static_cast<float>(fps.size() - 1) * FPS_FONT_SIZE, 30.0f + FPS_FONT_SIZE);
        ofPopStyle();
    }
}
