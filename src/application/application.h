#pragma once

#include "ImportImportExportScene.h"
#include "RotatingCubeScene.h"
#include "ofMain.h"
#include "renderer/renderer.h"

class Application : public ofBaseApp
{
public:
    Renderer renderer;

    void setup() override;
    void draw() override;
    void keyReleased(int key) override;
    void mouseReleased(int x, int y, int button) override;
    void exit() override;

    void SelectScene(SceneId id);

    ofxImGui::Gui gui{};

    std::shared_ptr<ImportImportExportScene> importExportImageScene;
    std::shared_ptr<RotatingCubeScene>       rotatingCubeScene;
};
