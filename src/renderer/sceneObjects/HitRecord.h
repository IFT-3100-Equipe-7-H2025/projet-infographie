#pragma once

#ifndef HITRECORD_H
#define HITRECORD_H

#include "ofMain.h"
#include "renderer/rayTracer/ray.h"

class Material;

class HitRecord
{
public:
    ofVec3f p;
    ofVec3f normal;
    shared_ptr<Material> mat;
    float t;
    double u;
    double v;
    bool front_face;

    void set_face_normal(const Ray& r, const ofVec3f& outward_normal)
    {
        front_face = r.getDirection().dot(outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};


#endif