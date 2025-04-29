#include "SceneGraph.h"

#include "Light.h"
#include "of3dPrimitives.h"
#include <stack>

SceneGraph::SceneGraph() : root(std::make_shared<Node>("World", std::make_shared<ofBoxPrimitive>(ofBoxPrimitive(0, 0, 0)))) {}

void SceneGraph::AddNode(std::shared_ptr<Node> node)
{
    root->AddChild(std::move(node));
}

void SceneGraph::Draw(const glm::vec3& lightPosition, const std::shared_ptr<Shader>& lightingModel) const
{
    root->Draw(lightPosition, lightingModel);
}

[[nodiscard]] std::shared_ptr<Node> SceneGraph::GetRoot() const
{
    return root;
}

[[nodiscard]] std::optional<std::shared_ptr<Node>> SceneGraph::GetNode(const NodeId id) const
{
    if (this->nodes.contains(id))
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

bool SceneGraph::IsNodeCurrentlyInGraph(NodeId id) const
{
    std::stack<Node*> traversal;
    traversal.push(root.get());

    while (!traversal.empty())
    {
        Node* current = traversal.top();
        traversal.pop();
        if (current->GetId() == id)
        {
            return true;
        }
        for (const auto& child: current->GetChildren())
        {
            traversal.push(child.get());
        }
    }
    return false;
}


std::vector<std::shared_ptr<Node>> SceneGraph::GetNodes() const
{
    std::stack<std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Node>> output;
    nodes.push(root);

    while (!nodes.empty())
    {
        std::shared_ptr<Node> current = nodes.top();
        output.push_back(current);
        nodes.pop();

        for (const auto& child: current->GetChildren())
        {
            nodes.push(child);
        }
    }

    return output;
}

bool SceneGraph::RemoveNode(NodeId id)
{
    if (id == this->root->GetId())
    {
        ofLogError() << "Cannot remove the root node";
        return false;
    }

    if (this->nodes.count(id))
    {
        if (std::shared_ptr<Node> node = this->nodes.at(id).lock(); node)
        {
            node->Delete();
        }
        this->nodes.erase(id);
        return true;
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
            return true;
        }

        for (const auto& child: current->GetChildren())
        {
            nodes.push(child.get());
        }
    }

    return false;
}
