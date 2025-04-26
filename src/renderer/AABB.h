#pragma once
#ifndef AABB_H
#define AABB_H

#include "Interval.h"
#include "Vec3.h"
#include "Ray.h"

class AABB
{
public:
    Interval x, y, z;

    AABB() {}

    AABB(const Interval& x, const Interval& y, const Interval& z) : x(x), y(y), z(z) {
        pad_to_minimums();
    }

    AABB(const Vec3& a, const Vec3& b) {

        x = (a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);
        pad_to_minimums();
    }

    AABB(const AABB& box0, const AABB& box1)
    {
        x = Interval(box0.x, box1.x);
        y = Interval(box0.y, box1.y);
        z = Interval(box0.z, box1.z);  
        pad_to_minimums();
    }

    const Interval& axis_interval(int n) const {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    bool hit(const Ray& r, Interval ray_t) const {
        const Vec3& ray_orig = r.getOrigin();
        const Vec3& ray_dir = r.getDirection();
        //ofLog() << x.min << y.min << z.min;
        //ofLog() << x.max << y.max << z.max;
        for (int axis = 0; axis < 3; axis++)
        {
            const Interval& ax = axis_interval(axis);
            const double adinv = 1.0 / ray_dir[axis];

            auto t0 = (ax.min - ray_orig[axis]) * adinv;
            auto t1 = (ax.max - ray_orig[axis]) * adinv;

            if (t0 < t1)
            {
                if (t0 > ray_t.min) ray_t.min = t0;
                if (t1 < ray_t.max) ray_t.max = t1;
            }
            else
            {
                if (t1 > ray_t.min) ray_t.min = t1;
                if (t0 < ray_t.max) ray_t.max = t0;
            }

            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }

private:
    void pad_to_minimums()
    {
        //WAS THIS( would make quicker) double delta = 0.0001;
        double delta = 0.01;
        if (x.size() < delta) x = x.expand(delta);
        if (y.size() < delta) y = y.expand(delta);
        if (z.size() < delta) z = z.expand(delta);
    }
    
};

#endif