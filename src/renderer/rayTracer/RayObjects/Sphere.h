
#include "Primitive3D.h"

//class Sphere : public Primitive3D
//{
//public:
//    Sphere(of3dPrimitive primitive, float radius, const shared_ptr<RayMaterial> material) : Primitive3D(primitive), center(getGlobalPosition()), radius(std::fmax(0,radius)) {
//        mat = material;
//        update();
//        
//    }
//
//    void update() {
//        center = getGlobalPosition();
//        rcenter = Ray(center, Vec3(0, 0, 0));
//        new_radius = radius * getScale().x;
//        auto rvec = Vec3(new_radius, new_radius, new_radius);
//        bbox = AABB(center - rvec, center + rvec);
//    }
//
//    AABB bounding_box() const override {return bbox;}
//
//    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) override
//    {
//        update();
//        auto oc = center - r.getOrigin();
//        auto a = r.getDirection().lengthSquared();
//        auto h = oc.dot(r.getDirection());
//        auto c = oc.lengthSquared() - new_radius * new_radius;
//        auto discriminant = h * h - a * c;
//
//        if (discriminant < 0)
//            return false;
//
//        auto sqrtd = std::sqrt(discriminant);
//
//        // Find the nearest root that lies in the acceptable range.
//        auto root = (h - sqrtd) / a;
//        if (root <= ray_t.min || ray_t.max <= root)
//        {
//            root = (h + sqrtd) / a;
//            if (root <= ray_t.min || ray_t.max <= root)
//                return false;
//        }
//
//        rec.t = root;
//        rec.p = r.at(rec.t);
//        ofVec3f outward_normal = (rec.p - center) / new_radius;
//        rec.set_face_normal(r, outward_normal);
//        rec.mat = mat;
//
//        return true;
//    }
//
//private:
//    ofVec3f center;
//    Ray rcenter;
//    float radius;
//    float new_radius;
//};


class Sphere : public Primitive3D
{
public :
  Sphere(of3dPrimitive primitive, float radius, shared_ptr<MaterialContainer> material) : Primitive3D(primitive), center(getGlobalPosition()), radiusX(std::fmax(0, radius)), radiusY(std::fmax(0, radius)), radiusZ(std::fmax(0, radius))
    {
        mat = material; 
        update();
    }

void update() override
{
    center = getGlobalPosition();
    rcenter = Ray(center, Vec3(0, 0, 0));
    new_radiusX = radiusX * getScale().x;
    new_radiusY = radiusY * getScale().y;
    new_radiusZ = radiusZ * getScale().z;
    auto rvec = Vec3(new_radiusX, new_radiusY, new_radiusZ);
    bbox = AABB(center - rvec, center + rvec);
}

AABB bounding_box() const override { return bbox; }

bool hit(const Ray& r, Interval ray_t, HitRecord& rec) override
{
    auto oc = center - r.getOrigin();

    ofVec3f scaled_origin(oc.x / new_radiusX, oc.y / new_radiusY, oc.z / new_radiusZ);
    ofVec3f scaled_direction(r.getDirection().x / new_radiusX, r.getDirection().y / new_radiusY, r.getDirection().z / new_radiusZ);


    auto a = scaled_direction.lengthSquared();
    auto h = scaled_origin.dot(scaled_direction);
    auto c = scaled_origin.lengthSquared() - 1;// because we're hitting a unit sphere after scaling

    //auto a = r.getDirection().lengthSquared();
    //auto h = oc.dot(r.getDirection());
    //auto c = oc.lengthSquared() - new_radius * new_radius;
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

    ofVec3f p_local = rec.p - center;
    ofVec3f outward_normal(
            p_local.x / (new_radiusX * new_radiusX),
            p_local.y / (new_radiusY * new_radiusY),
            p_local.z / (new_radiusZ * new_radiusZ));
    outward_normal.normalize();


    //ofVec3f outward_normal = (rec.p - center) / new_radius;


    rec.set_face_normal(r, outward_normal);
    rec.mat = mat->getMaterial();

    return true;
}

private:
ofVec3f center;
Ray rcenter;
float radiusX;
float new_radiusX;
float radiusY;
float new_radiusY;
float radiusZ;
float new_radiusZ;
}
;

