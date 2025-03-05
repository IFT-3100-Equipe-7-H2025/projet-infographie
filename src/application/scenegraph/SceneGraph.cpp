#include "SceneGraph.h"
#include "of3dPrimitives.h"
#include <stack>

SceneGraph::SceneGraph() : root(std::make_shared<Node>("World", std::make_shared<ofBoxPrimitive>(ofBoxPrimitive(0, 0, 0)))) {}

void SceneGraph::AddNode(std::shared_ptr<Node> node)
{
    root->AddChild(std::move(node));
}

void SceneGraph::Draw() const
{
    root->Draw();
}

[[nodiscard]] std::shared_ptr<Node> SceneGraph::GetRoot() const
{
    return root;
}

[[nodiscard]] std::optional<std::shared_ptr<Node>> SceneGraph::GetNode(NodeId id) const
{
    std::stack<Node*> nodes;
    nodes.push(root.get());

    while (!nodes.empty())
    {
        Node* current = nodes.top();
        nodes.pop();

        if (current->GetId() == id)
        {
            return std::shared_ptr<Node>(current);
        }

        for (const auto& child: current->GetChildren())
        {
            nodes.push(child.get());
        }
    }

    return std::nullopt;
}


std::vector<std::shared_ptr<Node>> SceneGraph::GetNodes() const
{
    std::stack<Node*> nodes;
    std::vector<std::shared_ptr<Node>> output;
    nodes.push(root.get());

    while (!nodes.empty())
    {
        Node* current = nodes.top();
        output.push_back(std::shared_ptr<Node>(current));
        nodes.pop();

        for (const auto& child: current->GetChildren())
        {
            nodes.push(child.get());
        }
    }

    return output;
}

void SceneGraph::RemoveNode(NodeId id)
{
    if (id == this->root->GetId())
    {
        ofLogError() << "Cannot remove the root node";
        return;
    }

    std::stack<Node*> nodes;
    nodes.push(root.get());

    while (!nodes.empty())
    {
        Node* current = nodes.top();
        nodes.pop();

        if (current->GetId() == id)
        {
            ofLog() << "Removing node " << current->GetName();
            current->Delete();
            return;
        }

        for (const auto& child: current->GetChildren())
        {
            nodes.push(child.get());
        }
    }
}
