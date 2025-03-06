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
    if (this->nodes.count(id))
    {
        if (std::shared_ptr<Node> node = this->nodes.at(id).lock(); node)
        {
            return node;
        }
        else
        {
            this->nodes.erase(id);
            return std::nullopt;
        }
    }

    std::stack<Node*> traversal;
    traversal.push(root.get());

    while (!traversal.empty())
    {
        Node* current = traversal.top();
        traversal.pop();

        if (current->GetId() == id)
        {
            std::shared_ptr<Node> found = current->shared_from_this();
            this->nodes[id] = found;
            return found;
        }

        for (const auto& child: current->GetChildren())
        {
            traversal.push(child.get());
        }
    }
    return std::nullopt;
}

void SceneGraph::RemoveNode(NodeId id)
{
    if (id == this->root->GetId())
    {
        ofLogError() << "Cannot remove the root node";
        return;
    }

    if (this->nodes.count(id))
    {
        if (std::shared_ptr<Node> node = this->nodes.at(id).lock(); node)
        {
            node->Delete();
        }
        this->nodes.erase(id);
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
            std::shared_ptr<Node> found = current->shared_from_this();
            found->Delete();
            this->nodes.erase(id);
            return;
        }

        for (const auto& child: current->GetChildren())
        {
            nodes.push(child.get());
        }
    }
}
