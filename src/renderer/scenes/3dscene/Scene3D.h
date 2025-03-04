#pragma once

#include "3dscene/commands/CommandHistory.h"
#include "Scene.h"
#include "createShapes/CreateShapeUI.h"
#include "createShapes/SharedShapeCreationParams.h"
#include "scenegraph/SceneGraph.h"
#include <ofxAssimpModelLoader.h>

class Scene3D : public Scene
{
public:
    Scene3D();

    void setup() override;

    void draw() override;

    std::string GetName() override { return "3D scene"; }

    void SelectNode(const std::shared_ptr<Node>& node);

    void AddNode(std::shared_ptr<Node> node) { sceneGraph.AddNode(std::move(node)); }
    void ShowChildren(const std::shared_ptr<Node>& node);

    void DrawSceneGraphWindow();
    void DrawSelectedNodeWindow();
    void DrawModifyNodeSliders(const std::shared_ptr<Node>& node);
    void DrawCommandHistoryWindow();

    void ResetParams(const std::shared_ptr<Node>& node);

private:
    CommandHistory history;

    SceneGraph sceneGraph;
    std::shared_ptr<std::shared_ptr<Node>> selectedNode;

    float translate[3] = {0.0f, 0.0f, 0.0f};
    glm::vec3 initialPosition;// Used to store the initial position of the selected node when using the sliders, so that we can undo the change in a single command

    float scale[3] = {1.0f, 1.0f, 1.0f};
    glm::vec3 initialScale;// Used to store the initial scale of the selected node when using the sliders, so that we can undo the change in a single command

    float rotate[3] = {0.0f, 0.0f, 0.0f};
    glm::quat initialRotation;// Used to store the initial rotation of the selected node when using the sliders, so that we can undo the change in a single command

    std::shared_ptr<SharedShapeCreationParams> sharedParams;

    std::vector<std::unique_ptr<CreateShapeUI>> createShapeUIs;

    ofMaterial material;
};
