#pragma once

#include "renderer/cursor/CursorManager.h"
#include "renderer/renderer.h"
#include "renderer/scenes/3dscene/Scene3D.h"
#include "renderer/scenes/ImportImageScene.h"
#include "renderer/scenes/PrimitiveScene.h"
#include "renderer/scenes/RotatingCubeScene.h"

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
