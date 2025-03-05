#pragma once

#include "GeometryScene.h"
#include "ImportImageScene.h"
#include "PrimitiveScene.h"
#include "RotatingCubeScene.h"
#include "ofMain.h"
#include "renderer/cursor/CursorManager.h"
#include "renderer/renderer.h"
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
    void update() override;
    void draw() override;
    void keyPressed(int key) override;
    void keyReleased(int key) override;
    void dragEvent(ofDragInfo dragInfo) override;

    void mouseReleased(int x, int y, int button) override;
    void mousePressed(int x, int y, int button) override;
    void mouseDragged(int x, int y, int button) override;
    void exit() override;

    void SelectScene(SceneId id);
    void ShowMainMenuBar();
    void ShowCaptureOption();

    void ExportImage(const std::string& path);
    void ExportImageButtonPressed();
    void StartTimedCapture(int, float);
    void CaptureManager();

    CursorManager cursorManager;

    ofxImGui::Gui gui{};

    std::shared_ptr<ImportImageScene> importExportImageScene;
    std::shared_ptr<RotatingCubeScene> rotatingCubeScene;
    std::shared_ptr<PrimitiveScene> primitiveScene;
    std::shared_ptr<Scene3D> scene3D;
    std::shared_ptr<GeometryScene> geometryScene;

    std::weak_ptr<Scene> selectedScene;

    private:
    bool capturing = false;
    bool gifCapturing = false;
    int screenshotCount = 0;
    float lastCaptureTime = 0.0f;
    float captureInterval = 0.5f;
    int totalScreenshots = 10;

    bool showPopup = false;
    int userScreenshots = 10;
    float userDuration = 5.0f;

    std::string exportPath;
};
