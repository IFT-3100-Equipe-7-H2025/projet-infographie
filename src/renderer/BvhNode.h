#pragma once
#ifndef BVHNODE_H
#define BVHNODE_H

#include "Interval.h"
#include "Vec3.h"
#include "Ray.h"
#include "ComposedShape.h"

#include "PrimitiveCreator.h"

class BvhNode : public Primitive3D
{
public:
    int depth = 50;
    BvhNode() {}

    BvhNode(ComposedShape& composed, int depth_p) : BvhNode(composed.objects, 0, composed.objects.size(), depth_p)
    {
        model = composed.getModel();
    }

    BvhNode(ComposedShape& composed) : BvhNode(composed.objects, 0, composed.objects.size(), depth)
    {
        model = composed.getModel();
    }

    BvhNode(vector<shared_ptr<SceneObject>>& objects, size_t start, size_t end, int depth_p) : depth(depth_p)
    {
        int axis = random_int(0, 2);

        auto comparator = (axis == 0)   ? box_x_compare 
                          : (axis == 1) ? box_y_compare
                                        : box_z_compare;

        size_t object_span = end - start;
   
        ofLog() << "Size : " << objects.size();
        if (object_span == 1) {
            ofLog() << "1 : " ;

            groups.push_back(objects[start]);
        }
        else if (object_span == 2) {
            ofLog() << "2 : ";

            groups.push_back(objects[start]);
            groups.push_back(objects[start + 1]);
        }
        else if (depth <= 0) {
            ofLog() << "Depth == 0";
            for (int i = start; i < end; i++)
            {
                groups.push_back(objects[i]);
            }
        }
        else {
            sort(begin(objects) + start, begin(objects) + end, comparator);

            auto mid = start + object_span / 2;
            shared_ptr<BvhNode> node1 = make_shared<BvhNode>(objects, start, mid, depth - 1);
            shared_ptr<BvhNode> node2 = make_shared<BvhNode>(objects, mid, end, depth - 1);
            groups.push_back(node1);
            groups.push_back(node2);
        }
        for (auto& object: groups)
        {
            AABB box1 = object->bounding_box();

            bbox = AABB(bbox, box1);
            //if (depth <= 41 && start == 0)
            //{

            //    ofLog() << "Depth : " << depth;
            //    ofLog() << "BBox in loop " << groups.size();

            //    ofLog() << " Gotten BBox" << box1.x.min << box1.y.min << box1.z.min;
            //    ofLog() << " Gotten BBox" << box1.x.max << box1.y.max << box1.z.max;

            //    ofLog() << "BBox" << bbox.x.min << bbox.y.min << bbox.z.min;
            //    ofLog() << "BBox" << bbox.x.max << bbox.y.max << bbox.z.max;
            //}
            
            
        }
        //if (depth < 42) {
        //    ofLog() << "Depth : " << depth;

        //    ofLog() << "BBox" << bbox.x.min << bbox.y.min << bbox.z.min;
        //    ofLog() << "BBox" << bbox.x.max << bbox.y.max << bbox.z.max;
        //    //bbox = AABB(left->bounding_box(), right->bounding_box());
        //}
       
    }


    void customDraw() override
    {
        return;
        for (auto& object: groups)
        {
            object->customDraw();
        }
        if (groups.size() > 2) { 
            return;
        }

        ofPushStyle();
        ofSetColor(ofColor(0, 255, 0));
        ofMesh boxMesh;
        boxMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINE_STRIP);
        boxMesh.addVertex(ofVec3f(bbox.x.min, bbox.y.min, bbox.z.min));
        boxMesh.addVertex(ofVec3f(bbox.x.min, bbox.y.min, bbox.z.max));
        boxMesh.addVertex(ofVec3f(bbox.x.min, bbox.y.max, bbox.z.max));
        boxMesh.addVertex(ofVec3f(bbox.x.min, bbox.y.max, bbox.z.min));


        boxMesh.addVertex(ofVec3f(bbox.x.min, bbox.y.min, bbox.z.min));
        boxMesh.addVertex(ofVec3f(bbox.x.max, bbox.y.min, bbox.z.min));
        boxMesh.addVertex(ofVec3f(bbox.x.max, bbox.y.max, bbox.z.min));
        boxMesh.addVertex(ofVec3f(bbox.x.min, bbox.y.max, bbox.z.min));
        boxMesh.addVertex(ofVec3f(bbox.x.min, bbox.y.min, bbox.z.min));

        boxMesh.addVertex(ofVec3f(bbox.x.min, bbox.y.min, bbox.z.min));
        boxMesh.addVertex(ofVec3f(bbox.x.max, bbox.y.min, bbox.z.min));
        boxMesh.addVertex(ofVec3f(bbox.x.max, bbox.y.min, bbox.z.max));
        boxMesh.addVertex(ofVec3f(bbox.x.min, bbox.y.min, bbox.z.max));

        boxMesh.addVertex(ofVec3f(bbox.x.max, bbox.y.min, bbox.z.max));
        boxMesh.addVertex(ofVec3f(bbox.x.max, bbox.y.min, bbox.z.min));
        boxMesh.addVertex(ofVec3f(bbox.x.max, bbox.y.min, bbox.z.max));
        boxMesh.addVertex(ofVec3f(bbox.x.max, bbox.y.max, bbox.z.max));
        boxMesh.addVertex(ofVec3f(bbox.x.max, bbox.y.max, bbox.z.min));
        boxMesh.addVertex(ofVec3f(bbox.x.max, bbox.y.min, bbox.z.min));
        boxMesh.addVertex(ofVec3f(bbox.x.min, bbox.y.min, bbox.z.min));

        boxMesh.addVertex(ofVec3f(bbox.x.min, bbox.y.min, bbox.z.max));
        boxMesh.addVertex(ofVec3f(bbox.x.max, bbox.y.min, bbox.z.max));
        boxMesh.addVertex(ofVec3f(bbox.x.max, bbox.y.max, bbox.z.max));
        boxMesh.addVertex(ofVec3f(bbox.x.min, bbox.y.max, bbox.z.max));
        boxMesh.addVertex(ofVec3f(bbox.x.min, bbox.y.min, bbox.z.max));
        boxMesh.addVertex(ofVec3f(bbox.x.min, bbox.y.min, bbox.z.min));

        boxMesh.addVertex(ofVec3f(bbox.x.min, bbox.y.max, bbox.z.min));
        boxMesh.addVertex(ofVec3f(bbox.x.max, bbox.y.max, bbox.z.min));
        boxMesh.addVertex(ofVec3f(bbox.x.max, bbox.y.max, bbox.z.max));
        boxMesh.addVertex(ofVec3f(bbox.x.min, bbox.y.max, bbox.z.max));



        boxMesh.draw();
        ofPopStyle();
    }

    bool hit(const Ray& r, Interval ray_t, hit_record& rec) override {
        if (!bbox.hit(r, ray_t))
            return false;

        bool hit_anything = false;
        float closest_so_far = ray_t.max;
        hit_record temp_rec;
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

    static bool box_compare(const shared_ptr<SceneObject> a, const shared_ptr<SceneObject> b, int axis_index) {
        auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
        auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
        return a_axis_interval.min < b_axis_interval.min;
    }

    static bool box_x_compare(const shared_ptr<SceneObject> a, const shared_ptr<SceneObject> b)
    {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare(const shared_ptr<SceneObject> a, const shared_ptr<SceneObject> b)
    {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare(const shared_ptr<SceneObject> a, const shared_ptr<SceneObject> b)
    {
        return box_compare(a, b, 2);
    }

private:
    //shared_ptr<SceneObject> left;
    //shared_ptr<SceneObject> right;
    vector<shared_ptr<SceneObject>> groups;
    vector<int> indices;
};

#endif