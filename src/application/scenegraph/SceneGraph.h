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

private:
    [[nodiscard]] std::optional<std::shared_ptr<Node>> GetNodeInternal(const std::shared_ptr<Node>& node, NodeId id) const;

    std::shared_ptr<Node> root;
};
