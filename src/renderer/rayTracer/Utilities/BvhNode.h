#pragma once

#include "Primitive3D.h"
#include "Vec3.h"
#include "rayTracer/RayObjects/ComposedShape.h"


class BvhNode : public Primitive3D
{
public:
    int depth;
    BvhNode() {}


    BvhNode(ComposedShape& composed, int depth_p) : BvhNode(composed.objects, 0, composed.objects.size(), depth_p)
    {
        //Primitive3D(composed);
        copy(composed);
        model = composed.getModel();
    }

    BvhNode(ComposedShape& composed) : BvhNode(composed, 50) {}

    BvhNode(vector<shared_ptr<Primitive3D>>& objects, size_t start, size_t end, int depth_p) : depth(depth_p)
    {
        bbox = AABB::empty;
        for (size_t object_index = start; object_index < end; object_index++)
        {
            auto& object = objects[object_index];
            bbox = AABB(bbox, object->bounding_box());
        }
        int axis = bbox.longest_axis();

        //int axis = random_int(0, 2);

        //ofLog() << "depth : " << depth_p;
        auto comparator = (axis == 0)   ? box_x_compare
                          : (axis == 1) ? box_y_compare
                                        : box_z_compare;

        object_span = end - start;

        //ofLog() << "Size : " << objects.size();
        //ofLog() << "object_span : " << object_span;
        if (object_span == 0)
        {
            return;
        }
        else if (object_span == 1)
        {
            groups.push_back(objects[start]);
        }
        else if (object_span == 2)
        {
            groups.push_back(objects[start]);
            groups.push_back(objects[start + 1]);
        }
        else if (depth <= 0)
        {
            //ofLog() << "Depth == 0";
            for (int i = start; i < end; i++)
            {
                groups.push_back(objects[i]);
            }
        }
        else
        {
            sort(begin(objects) + start, begin(objects) + end, comparator);

            auto mid = start + object_span / 2;
            shared_ptr<BvhNode> node1 = make_shared<BvhNode>(objects, start, mid, depth - 1);
            shared_ptr<BvhNode> node2 = make_shared<BvhNode>(objects, mid, end, depth - 1);
            groups.push_back(node1);
            groups.push_back(node2);
        }
    }

    void update() override
    {
        bbox = AABB();
        for (auto& object: groups)
        {
            object->update();
            bbox = AABB(bbox, object->bounding_box());
        }
    }

    void aabbDraw(ofMatrix4x4 transform) override
    {
        //return;
        for (auto& object: groups)
        {
            object->aabbDraw(transform);
        }
        Primitive3D::aabbDraw(transform);
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) override
    {
        if (!bbox.hit(r, ray_t))
            return false;

        bool hit_anything = false;
        float closest_so_far = ray_t.max;
        HitRecord temp_rec;
        for (auto& object: groups)
        {
            if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec))
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }
        return hit_anything;
    }

    static bool box_compare(const shared_ptr<Primitive3D> a, const shared_ptr<Primitive3D> b, int axis_index)
    {
        auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
        auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
        return a_axis_interval.min < b_axis_interval.min;
    }

    static bool box_x_compare(const shared_ptr<Primitive3D> a, const shared_ptr<Primitive3D> b)
    {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare(const shared_ptr<Primitive3D> a, const shared_ptr<Primitive3D> b)
    {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare(const shared_ptr<Primitive3D> a, const shared_ptr<Primitive3D> b)
    {
        return box_compare(a, b, 2);
    }

private:
    //shared_ptr<Primitive3D> left;
    //shared_ptr<Primitive3D> right;
    vector<shared_ptr<Primitive3D>> groups;
    vector<int> indices;
    size_t object_span;
};
