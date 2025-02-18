#pragma once

#include "Scene.h"
#include "scenegraph/SceneGraph.h"

class Scene3D : public Scene
{
public:
    Scene3D() : graph(SceneGraph()) {}

    void setup() override
    {
        this->SelectNode(this->graph.GetRoot());
    }

    void draw() override;

    std::string GetName() override { return "3D scene"; }

    void SelectNode(const std::shared_ptr<Node>& node)
    {
        translateX = node->GetInner()->getPosition().x;
        translateY = node->GetInner()->getPosition().y;
        selectedNode = node;
        node->SetOpen(!node->IsOpen());
    }

    void AddNode(std::shared_ptr<Node> node) { graph.AddNode(std::move(node)); }
    void ShowChildren(const std::shared_ptr<Node>& node);

private:
    SceneGraph graph;
    std::shared_ptr<Node> selectedNode = nullptr;

    float translateX = 0;
    float translateY = 0;
};
