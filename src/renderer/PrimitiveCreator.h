#pragma once

#include "ofMain.h"
#include <cmath>
#include <numbers>

class PrimitiveCreator
{
	public:
    static of3dPrimitive createTriangle();
    static of3dPrimitive createCube();
    static of3dPrimitive createSphere(int lat, int longi);
    static of3dPrimitive createLasagna(float l_w_ratio, int periods, int resolution_l, int resolution_w);
    static of3dPrimitive createPyramid(int sides);

};
