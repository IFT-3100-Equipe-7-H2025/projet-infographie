#include "SceneGraph.h"
#include "of3dPrimitives.h"

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
    if (root->GetId() == id)
    {
        return root;
    }
    else if (auto node = GetNodeInternal(root, id))
    {
        return node;
    }

    return std::nullopt;
}


[[nodiscard]] std::optional<std::shared_ptr<Node>> SceneGraph::GetNodeInternal(const std::shared_ptr<Node>& node, NodeId id) const
{
    for (const auto& child: node->GetChildren())
    {
        if (child->GetId() == id)
        {
            return child;
        }
        else if (auto node = GetNodeInternal(child, id))
        {
            return node;
        }
    }

    return std::nullopt;
}
