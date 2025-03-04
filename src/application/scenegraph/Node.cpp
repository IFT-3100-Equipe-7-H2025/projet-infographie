#include "Node.h"

Node::Node(std::string name, std::shared_ptr<ofNode> node) : inner(std::move(node)), name(std::move(name)), id(nextId++) {}

void Node::AddChild(std::shared_ptr<Node> node)
{
    if (node->GetId() == this->id)
    {
        throw std::runtime_error("Cannot add a node as a child of itself");
    }
    node->inner->setParent(*this->inner);
    children.push_back(std::move(node));
}

void Node::RemoveChild(NodeId id)
{
    for (auto it = children.begin(); it != children.end(); ++it)
    {
        if ((*it)->GetId() == id)
        {
            children.erase(it);
            return;
        }
    }
}

void Node::Draw() const
{
    if (inner)
    {
        inner->draw();
    }
    for (const auto& child: children)
    {
        child->Draw();
    }
}

[[nodiscard]] std::string Node::GetName() const { return name + " " + std::to_string(id); }

[[nodiscard]] const std::vector<std::shared_ptr<Node>>& Node::GetChildren() const { return children; }

[[nodiscard]] NodeId Node::GetId() const { return id; }

void Node::SetOpen(bool open) { isOpen = open; }

[[nodiscard]] bool Node::IsOpen() const { return isOpen; }

[[nodiscard]] const std::shared_ptr<ofNode>& Node::GetInner() const { return inner; }
