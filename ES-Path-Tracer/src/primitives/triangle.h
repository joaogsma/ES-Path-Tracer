#ifndef __GUARD_TRIANGLE_H__
#define __GUARD_TRIANGLE_H__

#include "vector3.h"

class Triangle {
public:
    const Vector3 p1, p2, p3;

    Triangle(Vector3 point1, Vector3 point2, Vector3 point3) : 
            p1(point1), p2(point2), p3(point3) {}

    // TODO...
    // 1. Triangle-ray intersection method
    // 2. Method to test if a given point is inside
};

#endif