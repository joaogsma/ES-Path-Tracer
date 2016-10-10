#ifndef __GUARD_RAY_H__
#define __GUARD_RAY_H__

#include "kd-tree/kd_tree.h"
#include "geometry/point3.h"
#include "geometry/triangle.h"
#include "geometry/vector3.h"

#include <vector>

class Ray {
public:
	const Point3 origin;
	const Vector3 direction;

	Ray(Point3 &origin, Vector3 &direction);

	// Compute the resulting point of the ray given parameter t
	Point3 at(double t) const;

	// Triangle intersection function
	bool hit(const Triangle &tri, double &t, 
		std::vector<double> &bar_weights) const;

	// KD-Tree Region intersection function
	bool hit(const Region& kd_region) const;

	bool contains_region(const Region& kd_region) const { return false; }
};

#endif
