#ifndef COMPOSEDSHAPE_H
#define COMPOSEDSHAPE_H



#include "Primitive3D.h"
#include "Vec3.h"
#include <cmath>
#include "Utility.h"
#include "Quad.h"

class ComposedShape : public Primitive3D
{
public:

    ComposedShape() {}

        
    ComposedShape(shared_ptr<Primitive3D> object, shared_ptr<RayMaterial> material): Primitive3D(*object)
    {
        mat = material;
        copy(*object);
        addShape(object); 
        update();
    }

    ComposedShape(const Vec3& size, shared_ptr<RayMaterial> material, of3dPrimitive primitive) : mat(material), size(size), Primitive3D(primitive, material)
    {
        mat = material;
        model = primitive;
        initialize();
        setShapes();
        update();

    }

    void customDraw() override
    {
        Primitive3D::customDraw();
        //for (auto& object : objects) {
        //    object->customDraw(getGlobalTransformMatrix());
        //}
    }



    void addShape(shared_ptr<Primitive3D> object) {
        objects.push_back(object);
        //still need to update when movement happens
        bbox = AABB(bbox, object->bounding_box());
    }


    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) override
    {
        //ofLog() << "Composed Shape hit";
        bool update_bbox = updateSettings();
        bool hit = false;
        float closest_so_far = ray_t.max;
        HitRecord temp_rec;
        for (auto& object : objects) {
            if (update_bbox)
            {
                object->update();
            }
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

    vector<shared_ptr<Primitive3D>> objects;


protected:


    Vec3 size;
    shared_ptr<RayMaterial> mat;

private:


};


#endif