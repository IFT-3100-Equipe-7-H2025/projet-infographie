#ifndef PYRAMID_H
#define PYRAMID_H


#include "Primitive3D.h"
#include "ComposedShape.h"
#include "Triangle.h"
#include "Material.h"
#include <numbers>


class Pyramid : public ComposedShape
{
public:

    Pyramid(int sides, const Vec3& size, shared_ptr<RayMaterial> mat, of3dPrimitive primitive) : ComposedShape(size, mat, primitive), sides(sides){
        setShapes();
    }
    void setShapes() override{
        float delta_theta = 2.0f * float(std::numbers::pi) / float(sides);

        Vec3 apex(0, -size.y, 0);
        Vec3 baseCenter(0, size.y, 0);

        for (int i = 0; i < sides; i++)
        {
            int next = (i + 1) % sides;
            float theta_i = static_cast<float>(i) * delta_theta;
            float theta_next = static_cast<float>(next) * delta_theta;
            Vec3 base_i(std::cos(theta_i) * size.x, size.y * 2, std::sin(theta_i) * size.z);
            Vec3 base_next(std::cos(theta_next) * size.x, size.y * 2, std::sin(theta_next) * size.z);
            Vec3 v0 = apex;
            Vec3 v1 = base_next;
            Vec3 v2 = base_i;
            
        }


        ofVec3f baseNormal(0, size.y, 0);
        for (int i = 0; i < sides; i++)
        {
            int next = (i + 1) % sides;
            float theta_i = static_cast<float>(i) * delta_theta;
            float theta_next = static_cast<float>(next) * delta_theta;
            Vec3 base_i(std::cos(theta_i) * size.x, 0, std::sin(theta_i) * size.z);
            Vec3 base_next(std::cos(theta_next) * size.x, 0, std::sin(theta_next) * size.z);
            shared_ptr<Triangle> triangle = make_shared<Triangle>(center, scale, orientation, baseCenter, base_i, base_next, mat);
            addShape(triangle);
        }





        //float half_size = size / 2;
        //Vec3 down(0, -size, 0);
        //Vec3 right(-size, 0, 0);
        //Vec3 back(0, 0, -size);
        //Vec3 temp(0, 0, 0);

        //for (int i = 0; i < sides; i++)
        //{
        //    shared_ptr<Triangle> newQuad = make_shared<Triangle>(center, scale, Vec3(half_size, half_size, half_size), down, right, mat);
        //    objects.push_back(newQuad);
        //    temp = down;
        //    down = right;
        //    right = back;
        //    back = temp;
        //}
        //down = -down;
        //right = -right;
        //back = -back;
        //half_size = -half_size;
    }

private:
    int sides;

};


#endif