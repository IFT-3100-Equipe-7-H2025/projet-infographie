#include "Node.h"
#include "of3dPrimitives.h"
#include <memory>

Node::Node(std::string name, std::shared_ptr<ofNode> node) : inner(std::move(node)), name(std::move(name)), id(nextId++) {}

void Node::AddChild(std::shared_ptr<Node> child)
{
    if (child->GetId() == this->id)
    {
        ofLogError() << "Cannot add a node as a child of itself";
        return;
    }
    child->inner->setParent(*this->inner);
    child->parent = shared_from_this();
    children.push_back(std::move(child));
}

void Node::RemoveChild(NodeId id)
{
    for (auto it = children.begin(); it != children.end(); ++it)
    {
        if ((*it)->GetId() == id)
        {
            (*it)->inner->clearParent();
            (*it)->parent.reset();
            children.erase(it);
            return;
        }
    }
}

void Node::Delete()
{
    for (const auto& child: this->children)
    {
        child->parent.reset();
    }

    if (!this->parent.expired())
    {
        this->parent.lock()->RemoveChild(this->id);
    }

    this->children.clear();
}

// NOLINTNEXTLINE(misc-no-recursion)
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
[[nodiscard]] const std::weak_ptr<Node>& Node::GetParent() const { return this->parent; }

void Node::SetOpen(bool open) { isOpen = open; }

[[nodiscard]] bool Node::IsOpen() const { return isOpen; }

[[nodiscard]] const std::shared_ptr<ofNode>& Node::GetInner() const { return inner; }
