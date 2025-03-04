#pragma once

#include "scenegraph/Node.h"

#include <memory>

struct SharedShapeCreationParams
{
    float r = 255.0f / 2.0f;
    float g = 255.0f / 2.0f;
    float b = 255.0f / 2.0f;
    std::shared_ptr<std::shared_ptr<Node>> selectedNode = std::make_shared<std::shared_ptr<Node>>(nullptr);
};
