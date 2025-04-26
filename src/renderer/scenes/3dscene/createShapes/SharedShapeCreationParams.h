#pragma once

#include "scenegraph/Node.h"
#include "Material.h"
#include <memory>


struct SharedShapeCreationParams
{
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};// Default color is white
    shared_ptr<Material> material = make_shared<Lambert>(Lambert(ofColor(0,200,0)));
    float fuzz = 0.0f;
    float refract = 1.0f;
    matType mat = LambertT;
    std::shared_ptr<std::shared_ptr<Node>> selectedNode = std::make_shared<std::shared_ptr<Node>>(nullptr);
};

