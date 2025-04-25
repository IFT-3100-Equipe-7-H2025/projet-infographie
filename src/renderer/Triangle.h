
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Primitive3D.h"
#include "Vec3.h"
#include <cmath>
#include "Utility.h"

class Triangle : public Primitive3D
{
public:
    Triangle(shared_ptr<ofVec3f> reference, shared_ptr<ofVec3f> scale, shared_ptr<ofQuaternion> orientation, const Vec3& Q, const Vec3& u, const Vec3& v, shared_ptr<Material> material) : 
        ve(v), ue(u), scale(scale), orient(orientation), mat(material), u(u), v(v), reference(reference), Q(Q), center(Q)
    {
        update();
    }

    static Triangle triangleFromPoints(shared_ptr<ofVec3f> reference, shared_ptr<ofVec3f> scale, shared_ptr<ofQuaternion> orientation, const Vec3& A, const Vec3& B, const Vec3& C, shared_ptr<Material> material)
    {
        Vec3 Q = A;
        Vec3 u = B - A;
        Vec3 v = C - A;

        Triangle triangle(reference, scale, orientation, Q, u, v, material);
        return triangle;
    }

    void update() {
        Q = *orient * *scale * center + *reference;
        u = *orient * ue * *scale;
        v = *orient * ve * *scale;
        auto n = u.getCrossed(v);
        normal = unit_vector(n);
        D = normal.dot(Q);
        w = n / n.dot(n);
    }


    bool hit(const Ray& r, Interval ray_t, hit_record& rec) override
    {
        update();
        auto denom = normal.dot(r.getDirection());

        // No hit if parallel
        if (std::fabs(denom) < 1e-8) {
            return false;
        }

        // Hit outside interval
        auto t = (D - normal.dot(r.getOrigin())) / denom;
        if (!ray_t.contains(t)) {
            return false;
        }

        auto intersection = r.at(t);

        if (!inTriangle(intersection, Q, Q + u, Q + v))
        {
            return false;
        }
            


        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);
        return true;
    }

    bool inTriangle(Vec3 P, Vec3 A, Vec3 B, Vec3 C) {
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
    ofVec3f center;
    Vec3 u, v, w;
    Vec3 ue, ve;
    shared_ptr<Material> mat;
    Vec3 normal;
    double D;
    shared_ptr<ofVec3f> reference;
    shared_ptr<ofVec3f> scale;
    shared_ptr<ofQuaternion> orient;

};

#endif