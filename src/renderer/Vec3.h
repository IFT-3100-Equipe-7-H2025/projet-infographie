#pragma once


#include "ofMain.h"
#include "Utility.h"

class Vec3: public ofVec3f
{
public:

    Vec3(ofVec3f vec) : ofVec3f(vec.x, vec.y, vec.z) {}

    Vec3(double x, double y, double z) : ofVec3f(x, y, z) {}
    

    double length_squared() const {
        return x * x + y * y + z * z;
    }

    static Vec3 random() {
        return Vec3(random_double(),random_double(), random_double());
    }

    static Vec3 random(double min, double max) {
        return Vec3((float) random_double(min, max), (float) random_double(min, max), (float) random_double(min, max));
    }

    
};

inline Vec3 unit_vector(const Vec3& v) {
    return Vec3(v / v.length());
}

inline Vec3 random_unit_vector() {
    while (true) {
        auto p = Vec3::random(-1, 1);
        auto lensq = p.length_squared();
        if (lensq <= 1 && 1e-160 < lensq) return p / sqrt(lensq);
    }
}

inline Vec3 random_on_hemisphere(const Vec3& normal) {
    Vec3 on_unit_sphere = random_unit_vector();
    if (on_unit_sphere.dot(normal) > 0.0)
        return on_unit_sphere;
    else {
        return -on_unit_sphere;
    }
}
