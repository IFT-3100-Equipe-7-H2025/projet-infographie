
#include "Primitive3D.h"

class Cube : public Primitive3D
{
public:
    bool hit(const Ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override
    {
        return false;
    }

private:
    ofVec3f center;
    double radius;
};
