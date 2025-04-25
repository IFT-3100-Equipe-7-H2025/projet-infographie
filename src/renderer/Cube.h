#ifndef CUBE_H
#define CUBE_H


#include "Primitive3D.h"
#include "ComposedShape.h"
#include "Quad.h"
#include "Material.h"

class Cube : public ComposedShape
{
public:

    Cube(const Vec3& size, shared_ptr<Material> mat, of3dPrimitive primitive) : ComposedShape(size, mat, primitive) {
        setShapes();
    }
    void setShapes() override
    {
        Vec3 half_size = size / 2;
        Vec3 down(0, -size.y, 0);
        Vec3 right(-size.x, 0, 0);
        Vec3 back(0, 0, -size.z);
        Vec3 temp(0, 0, 0);

        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < 3; i++)
            {
                shared_ptr<Quad> newQuad = make_shared<Quad>(center, scale, orientation, half_size, down, right, mat);
                objects.push_back(newQuad);
                temp = down;
                down = right;
                right = back;
                back = temp;
            }
            down = -down;
            right = -right;
            back = -back;
            half_size = -half_size;
        }
    }


};


#endif