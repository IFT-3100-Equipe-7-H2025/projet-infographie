#ifndef RAY_H
#define RAY_H

#include <cmath>
#include "ofMain.h"

class Ray
{
public:
   Ray() {};

   Ray(const ofVec3f& origin, const ofVec3f& direction)
       : origin(origin), direction(direction.normalized()) 
   {
   }

   const ofVec3f& getOrigin() const { return origin; }
   const ofVec3f& getDirection() const { return direction; }

   ofVec3f at(double t) const
   {
       return origin + t * direction;
   }

private:
   ofVec3f origin;
   ofVec3f direction;
};


#endif// RAY_H
