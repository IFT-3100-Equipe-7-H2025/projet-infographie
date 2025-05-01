#pragma once

#include "Node.h"
#include "Primitive3D.h"
#include "Quad.h"
#include "rayTracer/Utilities/AABB.h"
#include "rayTracer/Utilities/Vec3.h"
#include <cmath>
class ComposedShape : public Primitive3D
{
public:
    ComposedShape() {}


    ComposedShape(shared_ptr<Primitive3D> object, shared_ptr<MaterialContainer> material) : Primitive3D(*object)
    {
        mat = material;
        copy(*object);
        addShape(object);
        update();
    }

    ComposedShape(vector<shared_ptr<Primitive3D>> objects, shared_ptr<MaterialContainer> material)
    {
        mat = material;
        for (auto& object: objects)
        {
            addShape(object);
        }
        initialize();
        update();
    }

    ComposedShape(vector<shared_ptr<Node>> nodes)
    {
        updateSets = false;
        for (auto& node: nodes)
        {
            if (auto primitive3d = std::dynamic_pointer_cast<Primitive3D>(node->GetInner()); primitive3d)
            {
                auto object = primitive3d;
                addShape(object);
            }
        }
        initialize();
        update();
    }


    ComposedShape(const Vec3& size, shared_ptr<MaterialContainer> material, of3dPrimitive primitive) : size(size), Primitive3D(primitive, material)
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
    }

    void aabbDraw()
    {
        for (auto& object: objects)
        {
            object->aabbDraw(getGlobalTransformMatrix());
        }
    }

    void aabbDraw(ofMatrix4x4 transform) override
    {
        for (auto& object: objects)
        {
            object->aabbDraw(transform);
        }
    }


    void addShape(shared_ptr<Primitive3D> object)
    {
        objects.push_back(object);
        //still need to update when movement happens
        bbox = AABB(bbox, object->bounding_box());
    }


    void update() override
    {
        bool update_bbox = updateSettings();
        bbox = AABB();
        for (auto& object: objects)
        {
            object->update();
            bbox = AABB(bbox, object->bounding_box());
        }
    }


    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) override
    {
        bool hit = false;
        float closest_so_far = ray_t.max;
        HitRecord temp_rec;
        for (auto& object: objects)
        {
            if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec))
            {
                hit = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }
        return hit;
    }

    virtual void setShapes() {}

    vector<shared_ptr<Primitive3D>> objects;


protected:
    Vec3 size;
    bool updateSets = true;

private:
};
