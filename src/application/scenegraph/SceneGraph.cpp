#include "SceneGraph.h"

#include "Light.h"
#include "of3dPrimitives.h"
#include <stack>

SceneGraph::SceneGraph() : root(std::make_shared<Node>("World", std::make_shared<ofBoxPrimitive>(ofBoxPrimitive(0, 0, 0)))) {}

void SceneGraph::AddNode(std::shared_ptr<Node> node)
{
    root->AddChild(std::move(node));
}

void SceneGraph::Draw(const std::shared_ptr<Shader>& lightingModel, const glm::vec3& lightPosition) const
{
    root->Draw(lightingModel, lightPosition);
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


SceneGraph SceneGraph::clone() const {
    SceneGraph copy;

    std::stack<std::shared_ptr<Node>> nodes;
    std::stack<std::shared_ptr<Node>> nodes_copy;


    //std::shared_ptr<Node> root_copy = std::make_shared<Node>(root->GetName(), std::make_shared<ofNode>(*root->GetInner()));
    std::shared_ptr<Node> root_copy = copy.GetRoot();

    nodes.push(root);
    nodes_copy.push(root_copy);

    while (!nodes.empty())
    {
        std::shared_ptr<Node> current = nodes.top();
        std::shared_ptr<Node> current_copy = nodes_copy.top();
        nodes.pop();
        nodes_copy.pop();

        for (const auto& child: current->GetChildren())
        {
            std::shared_ptr<Node> child_copy = std::make_shared<Node>(child->GetName(), std::make_shared<ofNode>(*child->GetInner()));
            nodes.push(child);
            nodes_copy.push(child_copy);
            current_copy->AddChild(child_copy);
        }
    }

    //copy.AddNode(root_copy);
    return copy;
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
