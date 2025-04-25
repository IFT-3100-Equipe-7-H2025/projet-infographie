
#include "Primitive3D.h"

class Sphere : public Primitive3D
{
public:
    Sphere(of3dPrimitive primitive, float radius, shared_ptr<Material> material) : Primitive3D(primitive), center(getGlobalPosition()), radius(std::fmax(0,radius)) {
        mat = material;
        update();
        
    }

    void update() {
        center = getGlobalPosition();
        rcenter = Ray(center, Vec3(0, 0, 0));
        new_radius = radius * getScale().x;
        auto rvec = Vec3(new_radius, new_radius, new_radius);
        bbox = AABB(center - rvec, center + rvec);
    }

    AABB bounding_box() const override {return bbox;}

    bool hit(const Ray& r, Interval ray_t, hit_record& rec) override
    {
        update();
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
        if (root <= ray_t.min || ray_t.max <= root)
        {
            root = (h + sqrtd) / a;
            if (root <= ray_t.min || ray_t.max <= root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        ofVec3f outward_normal = (rec.p - center) / new_radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

private:
    ofVec3f center;
    Ray rcenter;
    float radius;
    float new_radius;
};

