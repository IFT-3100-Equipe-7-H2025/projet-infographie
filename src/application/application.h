#pragma once

#include "ImportImageScene.h"
#include "RotatingCubeScene.h"
#include "ofMain.h"
#include "renderer/renderer.h"
#include "PrimitiveScene.h"
#include "renderer/cursor/CursorManager.h"

class Application : public ofBaseApp
{
public:
    Renderer renderer;

    void setup() override;
    void draw() override;
    void keyReleased(int key) override;
    void mouseReleased(int x, int y, int button) override;
    void mousePressed(int x, int y, int button) override;
    void exit() override;

    void SelectScene(SceneId id);
    void ShowMainMenuBar();

    void ExportImage(const std::string& path);
    void ExportImageButtonPressed();

    CursorManager cursorManager;

    ofxImGui::Gui gui{};

    std::shared_ptr<ImportImageScene> importExportImageScene;
    std::shared_ptr<RotatingCubeScene> rotatingCubeScene;
    std::shared_ptr<PrimitiveScene> primitiveScene;
};
