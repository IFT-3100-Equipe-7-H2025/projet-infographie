#pragma once

#include "Light.h"
#include "scenegraph/Node.h"
#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <vector>


class SceneGraph
{
public:
    SceneGraph();

    void Draw(const std::vector<std::shared_ptr<Light>>& lights, const std::shared_ptr<Shader>& lightingModel = nullptr) const;

    void AddNode(std::shared_ptr<Node> node);
    [[nodiscard]] std::shared_ptr<Node> GetRoot() const;
    [[nodiscard]] std::optional<std::shared_ptr<Node>> GetNode(NodeId id) const;
    std::vector<std::shared_ptr<Node>> GetNodes() const;
    SceneGraph clone() const;

    bool IsNodeCurrentlyInGraph(NodeId id) const;

    bool RemoveNode(NodeId id);

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

    std::shared_ptr<Node> root;
    mutable std::unordered_map<NodeId, std::weak_ptr<Node>> nodes;
};
