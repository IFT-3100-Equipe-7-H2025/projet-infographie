#pragma once

#include "scenegraph/Node.h"
#include <glm/glm.hpp>
#include <memory>
#include <optional>


class SceneGraph
{
public:
    SceneGraph();

    void Draw() const;

    void AddNode(std::shared_ptr<Node> node);
    [[nodiscard]] std::shared_ptr<Node> GetRoot() const;
    [[nodiscard]] std::optional<std::shared_ptr<Node>> GetNode(NodeId id) const;
    std::vector<std::shared_ptr<Node>> GetNodes() const;

    void RemoveNode(NodeId id);

    void DebugPrint() const
    {
        this->DebugPrintInner(root);
    }

private:
    // NOLINTNEXTLINE(misc-no-recursion)
    void DebugPrintInner(const std::shared_ptr<Node>& node, int indent = 0) const
    {
        ofLog() << std::string(indent, ' ') << node->GetName();
        for (const auto& child: node->GetChildren())
        {
            DebugPrintInner(child, indent + 1);
        }
    }

    [[nodiscard]] std::optional<std::shared_ptr<Node>> GetNodeInternal(const std::shared_ptr<Node>& node, NodeId id) const;

    std::shared_ptr<Node> root;
};
