#pragma once

#include "of3dPrimitives.h"
#include "ofNode.h"

typedef int NodeId;

class Node
{
public:
    explicit Node(std::string name, std::shared_ptr<of3dPrimitive> primitive);

    void AddChild(std::shared_ptr<Node> node);

    void Draw() const;

    [[nodiscard]] const std::string& GetName() const;

    [[nodiscard]] const std::vector<std::shared_ptr<Node>>& GetChildren() const;

    [[nodiscard]] NodeId GetId() const;

    void SetOpen(bool open);

    [[nodiscard]] bool IsOpen() const;

    [[nodiscard]] const std::shared_ptr<ofNode>& GetInner() const;

private:
    std::shared_ptr<ofNode> inner;
    std::vector<std::shared_ptr<Node>> children;
    std::string name;
    int id;
    bool isOpen = false;

    static NodeId nextId;
};

inline NodeId Node::nextId = 0;
