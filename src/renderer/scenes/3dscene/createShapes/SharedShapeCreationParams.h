#pragma once

#include "scenegraph/Node.h"

#include <memory>
enum matType
{
    Metal,
    Glass,
    Lambert
};

struct SharedShapeCreationParams
{
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};// Default color is white
    matType material = matType::Lambert;
    bool isGlass = false;
    std::shared_ptr<std::shared_ptr<Node>> selectedNode = std::make_shared<std::shared_ptr<Node>>(nullptr);
};
