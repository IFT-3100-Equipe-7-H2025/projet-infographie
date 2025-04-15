
#include "Primitive3D.h"

class Sphere : public Primitive3D
{
public:
    Sphere(of3dPrimitive primitive, float radius) : Primitive3D(primitive), radius(radius)
    {
        center = getGlobalPosition();
    }

    bool hit(const Ray& r, double ray_tmin, double ray_tmax, hit_record& rec) override
    {
        center = getGlobalPosition();
        float new_radius = radius * getScale().x;
        auto oc = center - r.getOrigin();
        auto a = r.getDirection().lengthSquared();
        auto h = oc.dot(r.getDirection());
        auto c = oc.lengthSquared() - new_radius * new_radius;
        auto discriminant = h * h - a * c;

        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (root <= ray_tmin || ray_tmax <= root)
        {
            root = (h + sqrtd) / a;
            if (root <= ray_tmin || ray_tmax <= root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        ofVec3f outward_normal = (rec.p - center) / new_radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

private:
    ofVec3f center;
    float radius;
};

