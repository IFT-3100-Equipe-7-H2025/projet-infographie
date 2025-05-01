
#ifndef QUAD_H
#define QUAD_H

#include "Primitive3D.h"
#include "rayTracer/Utilities/Vec3.h"
#include <cmath>

class Quad : public Primitive3D
{
public:
    Quad(shared_ptr<ofVec3f> reference, shared_ptr<ofVec3f> scale, shared_ptr<ofQuaternion> orientation, const Vec3& Q, const Vec3& u, const Vec3& v, shared_ptr<MaterialContainer> material) : 
        ve(v), ue(u), scale(scale), orient(orientation), u(u), v(v), reference(reference), Q(Q), corner(Q)
    {
        mat = material;
        update();
    }

    void update() override
    {
        Q = *orient * *scale * corner + *reference;
        u = *orient * ue * *scale;
        v = *orient * ve * *scale;
        auto n = u.getCrossed(v);
        normal = unit_vector(n);
        D = normal.dot(Q);
        w = n / n.dot(n);
    }


    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) override
    {
        update();
        auto denom = normal.dot(r.getDirection());

        if (std::fabs(denom) < 1e-8) {
            return false;
        }

        // Hit outside interval
        auto t = (D - normal.dot(r.getOrigin())) / denom;
        if (!ray_t.contains(t)) {
            return false;
        }

        auto intersection = r.at(t);
        Vec3 planar_hitpt_vec = intersection - Q;

        Vec3 aCross = planar_hitpt_vec.getCrossed(v);
        Vec3 bCross = u.getCrossed(planar_hitpt_vec);

        auto alpha = w.dot(planar_hitpt_vec.getCrossed(v));
        auto beta = w.dot(u.getCrossed(planar_hitpt_vec));

        if (!is_interior(alpha, beta, rec)) {
            return false;
        }
           
        rec.t = t;
        rec.p = intersection;
        rec.mat = mat->getMaterial();
        rec.set_face_normal(r, normal);
        return true;
    }

    virtual bool is_interior(double a, double b, HitRecord& rec) const {
        Interval unit_interval = Interval(0, 1);

        if (!unit_interval.contains(a) || !unit_interval.contains(b))
            return false;
        
        rec.u = a;
        rec.v = b;

        return true;
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

#endif