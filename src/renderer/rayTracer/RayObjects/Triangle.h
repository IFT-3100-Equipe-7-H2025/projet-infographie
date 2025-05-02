#pragma once

#include "Primitive3D.h"
#include <cmath>

class Triangle : public Primitive3D
{
public:
    Triangle(shared_ptr<ofVec3f>& reference, shared_ptr<ofVec3f>& scale, shared_ptr<ofQuaternion>& orientation, const Vec3& Q, const Vec3& u, const Vec3& v, shared_ptr<MaterialContainer> material) : ve(v), ue(u), scale(scale), orient(orientation), u(u), v(v), reference(reference), Q(Q), corner(Q)
    {
        mat = material;
        update();
    }

    static Triangle triangleFromPoints(shared_ptr<ofVec3f>& reference, shared_ptr<ofVec3f>& scale, shared_ptr<ofQuaternion>& orientation, const Vec3& A, const Vec3& B, const Vec3& C, shared_ptr<MaterialContainer> material)
    {
        Vec3 Q = A;
        Vec3 u = B - A;
        Vec3 v = C - A;

        Triangle triangle(reference, scale, orientation, Q, u, v, material);
        return triangle;
    }

    Triangle(const Triangle& other)
        : reference(other.reference),
          scale(other.scale),
          orient(other.orient),
          ue(other.ue),
          ve(other.ve),
          u(other.u),
          v(other.v),
          Q(other.Q),
          corner(other.corner)
    {
        mat = other.mat;
        bbox = other.bbox;
        model = other.model;
        update();
    }



    void update() override{
        Q = *orient * (*scale * corner) + *reference;//*orient *
        u = *orient * (*scale * ue);
        v = *orient * (*scale * ve);
        auto n = u.getCrossed(v);
        normal = unit_vector(n);
        D = normal.dot(Q);
        w = n / n.dot(n);
        calculateBbox();
    }

    void calculateBbox()
    {
        auto rvecMin = ofVec3f(min(min(Q.x, Q.x + u.x), Q.x + v.x), min(min(Q.y, Q.y + u.y), Q.y + v.y), min(min(Q.z, Q.z + u.z), Q.z + v.z));
        auto rvecMax = ofVec3f(max(max(Q.x, Q.x + u.x), Q.x + v.x), max(max(Q.y, Q.y + u.y), Q.y + v.y), max(max(Q.z, Q.z + u.z), Q.z + v.z));

        bbox = AABB(rvecMin, rvecMax);
    }

    AABB bounding_box() const override { return bbox; }


    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) override
    {
        //update();
        auto denom = normal.dot(r.getDirection());

        // No hit if parallel
        if (std::fabs(denom) < 1e-8)
        {
            return false;
        }

        // Hit outside interval
        auto t = (D - normal.dot(r.getOrigin())) / denom;
        if (!ray_t.contains(t))
        {
            return false;
        }

        auto intersection = r.at(t);

        if (!inTriangle(intersection, Q, Q + u, Q + v))
        {
            return false;
        }


        rec.t = t;
        rec.p = intersection;
        rec.mat = mat->getMaterial();
        rec.set_face_normal(r, normal);
        return true;
    }

    bool inTriangle(Vec3 P, Vec3 A, Vec3 B, Vec3 C)
    {
        Vec3 v0 = C - A;
        Vec3 v1 = B - A;
        Vec3 v2 = P - A;

        float d00 = v0.dot(v0);
        float d01 = v0.dot(v1);
        float d11 = v1.dot(v1);
        float d20 = v2.dot(v0);
        float d21 = v2.dot(v1);

        float denom = d00 * d11 - d01 * d01;
        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        float u = 1.0f - v - w;

        return (u >= 0) && (v >= 0) && (w >= 0);
    }

private:
    ofVec3f Q;
    ofVec3f corner;
    Vec3 u, v, w;
    Vec3 ue, ve;
    Vec3 normal;
    double D;
    shared_ptr<ofVec3f> reference;
    shared_ptr<ofVec3f> scale;
    shared_ptr<ofQuaternion> orient;
};
