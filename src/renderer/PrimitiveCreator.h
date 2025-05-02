#pragma once

#include "exprtk.hpp"
#include "of3dPrimitives.h"
#include <WallPrimitive.h>
#include <cmath>
#include <rayTracer/RayMaterial.h>
#include <rayTracer/RayObjects/RayMesh.h>

class PrimitiveCreator
{
public:
    static of3dPrimitive createTriangle();
    static of3dPrimitive createCube(float width = 1.0f, float height = 1.0f, float depth = 1.0f);
    static RayMesh createSphere(int lat, int longi, float radius, std::shared_ptr<MaterialContainer> mat);
    static of3dPrimitive createSphere(int lat, int longi, float radius);
    static of3dPrimitive createLasagna(float l_w_ratio, int periods, int resolution_l, int resolution_w, float w = 1.0f, float h = 1.0f, float d = 1.0f);
    static of3dPrimitive createPyramid(int sides, float width = 1.0f, float height = 1.0f, float depth = 1.0f);
    static of3dPrimitive createSurface(float starting_u, float end_u, float starting_v, float end_v, int resolution, std::string x, std::string y, std::string z);
    static of3dPrimitive createWall(const std::string& heightmapPath, float wallWidth, float wallHeight, float wallDepth, float heightmapScale, int resX, int resY);
};
