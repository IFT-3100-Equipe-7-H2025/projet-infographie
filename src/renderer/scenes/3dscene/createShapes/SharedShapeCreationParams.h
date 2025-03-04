#pragma once

#include "scenegraph/Node.h"

#include <memory>

struct SharedShapeCreationParams
{
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};// Default color is white
    std::shared_ptr<std::shared_ptr<Node>> selectedNode = std::make_shared<std::shared_ptr<Node>>(nullptr);
};
