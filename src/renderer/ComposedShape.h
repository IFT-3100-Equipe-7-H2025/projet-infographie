#ifndef QUADSHAPE_H
#define QUADSHAPE_H



#include "Primitive3D.h"
#include "Vec3.h"
#include <cmath>
#include "Utility.h"
#include "Quad.h"

class ComposedShape : public Primitive3D
{
public:

    ComposedShape(const Vec3& size, shared_ptr<Material> mat, of3dPrimitive primitive) : mat(mat), size(size), Primitive3D(primitive) {
        center = make_shared<ofVec3f>(0, 0, 0);
        scale = make_shared<ofVec3f>(0, 0, 0);
        orientation = make_shared<ofQuaternion>();
        setShapes();
    }

    void update() {
        *center = getGlobalPosition();
        *scale = getScale();
        *orientation = getOrientationQuat();
    }


    bool hit(const Ray& r, Interval ray_t, hit_record& rec) override
    {
        update();
        bool hit = false;
        float closest_so_far = ray_t.max;
        hit_record temp_rec;
        for (auto& object : objects) {
            if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec))
            {
                hit = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }
        return hit;
    }



    virtual void setShapes() {};

protected:

    vector<shared_ptr<SceneObject>> objects;
    shared_ptr<ofVec3f> center;
    shared_ptr<ofVec3f> scale;
    shared_ptr<ofQuaternion> orientation;
    Vec3 size;
    shared_ptr<Material> mat;

};


#endif