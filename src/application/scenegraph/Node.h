#pragma once

#include "Light.h"
#include "Material.h"
#include "Shader.h"
#include "ofNode.h"

typedef unsigned int NodeId;

class Node : public std::enable_shared_from_this<Node>
{
public:
    explicit Node(std::string name, std::shared_ptr<ofNode> node);
    ~Node()
    {
        ofLogVerbose() << "~ Deleting node {" << this->GetName() << "}";
    }

    bool AddChild(std::shared_ptr<Node> child);
    void RemoveChild(NodeId id);

    bool Delete();

    void Draw(const std::shared_ptr<Shader>& lightingModel = nullptr) const;

    void SetMaterial(std::shared_ptr<Material> material);
    [[nodiscard]] const std::shared_ptr<Material>& GetMaterial() const;

    [[nodiscard]] std::string GetName() const;
    [[nodiscard]] const std::vector<std::shared_ptr<Node>>& GetChildren() const;
    [[nodiscard]] NodeId GetId() const;
    [[nodiscard]] const std::weak_ptr<Node>& GetParent() const;

    void SetOpen(bool open);
    [[nodiscard]] bool IsOpen() const;

    [[nodiscard]] const std::shared_ptr<ofNode>& GetInner() const;

private:
    friend class SceneGraph;

    std::shared_ptr<ofNode>            inner;
    std::weak_ptr<Node>                parent;
    std::vector<std::shared_ptr<Node>> children;

    std::shared_ptr<Material> material = std::make_shared<Material>(DEFAULT_MATERIAL);

    std::string name;
    int id;
    bool isOpen = false;

    static NodeId nextId;
};

inline NodeId Node::nextId = 0;
