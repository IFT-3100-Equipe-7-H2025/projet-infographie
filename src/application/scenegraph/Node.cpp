#include "Node.h"

Node::Node(std::string name, std::shared_ptr<of3dPrimitive> primitive) : inner(std::move(primitive)), name(std::move(name)), id(nextId++) {}

void Node::AddChild(std::shared_ptr<Node> node)
{
    node->inner->setParent(*this->inner);
    children.push_back(std::move(node));
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
