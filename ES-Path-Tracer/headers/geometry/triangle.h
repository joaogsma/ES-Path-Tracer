#ifndef __GUARD_TRIANGLE_H__
#define __GUARD_TRIANGLE_H__

#include "geometry/point3.h"
#include "geometry/surface_point.h"
#include "geometry/vector3.h"

#include <memory>
#include <vector>

class Triangle {
public:
    std::shared_ptr<Surface_Point> v1, v2, v3;

	Triangle(const std::shared_ptr<Surface_Point> &point1, 
		const std::shared_ptr<Surface_Point> &point2, 
		const std::shared_ptr<Surface_Point> &point3);

	std::vector<double> baricentric_coordinates(const Point3& p) const;
};

#endif