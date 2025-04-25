#pragma once

#include "of3dPrimitives.h"
#include "RayMesh.h"
#include <cmath>

class PrimitiveCreator
{
public:
    static of3dPrimitive createTriangle();
    static of3dPrimitive createCube(float width = 1.0f, float height = 1.0f, float depth = 1.0f);
    static RayMesh createSphere(int lat, int longi, float radius, shared_ptr<Material> mat);
    static of3dPrimitive createSphere(int lat, int longi, float radius);
    static of3dPrimitive createLasagna(float l_w_ratio, int periods, int resolution_l, int resolution_w, float w = 1.0f, float h = 1.0f, float d = 1.0f);
    static of3dPrimitive createPyramid(int sides, float width = 1.0f, float height = 1.0f, float depth = 1.0f);
};
