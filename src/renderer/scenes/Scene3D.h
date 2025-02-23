#pragma once

#include "Scene.h"
#include "createShapes/CreateCubeUI.h"
#include "createShapes/CreateShapeUI.h"
#include "createShapes/SharedShapeCreationParams.h"
#include "scenegraph/SceneGraph.h"

class Scene3D : public Scene
{
public:
    Scene3D() : graph(SceneGraph())
              , selectedNode(std::make_shared<std::shared_ptr<Node>>(nullptr))
              , sharedParams(std::make_shared<SharedShapeCreationParams>()) { sharedParams->selectedNode = selectedNode; }

    void setup() override
    {
        this->SelectNode(this->graph.GetRoot());

        createShapeUIs.push_back(std::make_unique<CreateCubeUI>(CreateCubeUI(sharedParams)));
    }

    void draw() override;

    std::string GetName() override { return "3D scene"; }

    void SelectNode(const std::shared_ptr<Node>& node)
    {
        *selectedNode = node;

        translateX = node->GetInner()->getPosition().x;
        translateY = node->GetInner()->getPosition().y;

        node->SetOpen(!node->IsOpen());
    }

    void AddNode(std::shared_ptr<Node> node) { graph.AddNode(std::move(node)); }
    void ShowChildren(const std::shared_ptr<Node>& node);

private:
    SceneGraph                             graph;
    std::shared_ptr<std::shared_ptr<Node>> selectedNode;

    float translateX = 0.0f;
    float translateY = 0.0f;

    std::shared_ptr<SharedShapeCreationParams> sharedParams;

    std::vector<std::unique_ptr<CreateShapeUI>> createShapeUIs;
};
