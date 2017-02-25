#ifndef __GUARD_PLANE_H__
#define __GUARD_PLANE_H__

#include "point3.h"
#include "vector3.h"

class Plane {
public:
    Point3 point;
    Vector3 normal;

    inline Plane(const Point3& point, const Vector3& normal) : point(point), normal(normal) {}

    inline Plane(const Point3& point1, const Point3& point2, const Point3& point3);

    double evaluate(const Point3& point) const;
};

#endif
