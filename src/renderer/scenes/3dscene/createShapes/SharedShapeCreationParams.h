#pragma once

#include "rayTracer/RayMaterial.h"
#include "scenegraph/Node.h"

#include <memory>


struct SharedShapeCreationParams
{
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};// Default color is white
    bool useBVH = true;
    shared_ptr<MaterialContainer> material = make_shared<MaterialContainer>(make_shared<Lambert>(Lambert(Vec3(0,200,0))));
    float fuzz = 0.0f;
    float refract = 1.0f;
    matType mat = LambertT;
    std::shared_ptr<std::shared_ptr<Node>> selectedNode = std::make_shared<std::shared_ptr<Node>>(nullptr);
};
