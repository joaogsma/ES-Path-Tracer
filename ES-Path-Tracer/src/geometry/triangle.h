#ifndef __GUARD_TRIANGLE_H__
#define __GUARD_TRIANGLE_H__

#include "point3.h"
#include "ray.h"
#include "surface_point.h"
#include "vector3.h"

#include <vector>

class Triangle {
public:
    const Surface_Point v1, v2, v3;

	Triangle(const Surface_Point &point1, const Surface_Point &point2, const Surface_Point &point3);

	bool intersect(const Ray &ray, double &t, std::vector<double> &weights) const;

	std::vector<double> baricentric_coordinates(const Point3& p) const;
};

#endif