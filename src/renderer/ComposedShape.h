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

    ComposedShape(shared_ptr<SceneObject> object, shared_ptr<Material> mat)
    {
        addShape(object); 
        if (auto object3d = std::dynamic_pointer_cast<Primitive3D>(object); object3d)
        {
            model = object3d->getModel();
        }
        initialize();
        update();

    }

    ComposedShape(const Vec3& size, shared_ptr<Material> mat, of3dPrimitive primitive) : mat(mat), size(size), Primitive3D(primitive) {
        model = primitive;
        initialize();
        setShapes();
        update();

    }

    void initialize() {
        center = make_shared<ofVec3f>(0, 0, 0);
        scale = make_shared<ofVec3f>(1, 1, 1);
        orientation = make_shared<ofQuaternion>();

    }

     void customDraw() override
    {
         Primitive3D::customDraw();
         for (auto& object : objects) {
             object->customDraw();
         }
    }

    void update() {
        *center = getGlobalPosition();
        *scale = getScale();
        *orientation = getOrientationQuat();
    }

    void addShape(shared_ptr<SceneObject> object) {
        objects.push_back(object);
        //still need to update when movement happens
        bbox = AABB(bbox, object->bounding_box());
    }


    bool hit(const Ray& r, Interval ray_t, hit_record& rec) override
    {
        //ofLog() << "Composed Shape hit";
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

    //vector<shared_ptr<SceneObject>>& getObjects() {
    //    return objects;
    //};

    virtual void setShapes() {};

    vector<shared_ptr<SceneObject>> objects;


protected:

    shared_ptr<ofVec3f> center;
    shared_ptr<ofVec3f> scale;
    shared_ptr<ofQuaternion> orientation;
    Vec3 size;
    shared_ptr<Material> mat;

private:


};


#endif