#pragma once

#include "ImportImageScene.h"
#include "PrimitiveScene.h"
#include "RotatingCubeScene.h"
#include "Scene3D.h"
#include "renderer/cursor/CursorManager.h"
#include "renderer/renderer.h"

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
    std::shared_ptr<Scene3D> scene3D;
};
