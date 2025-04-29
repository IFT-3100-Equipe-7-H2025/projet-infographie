#pragma once
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>

const double pi = 3.14159265358979323846;


inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}


inline double random_double()
{
    return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max)
{
    return min + (max - min) * random_double();
}

inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0) {
        return sqrt(linear_component);
    }

    return 0;
}

inline int random_int(int min, int max) {
    return int(random_double(min, max + 1));
}
