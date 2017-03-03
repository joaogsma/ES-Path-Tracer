#ifndef __GUARD_RAY_H__
#define __GUARD_RAY_H__

#include "geometry/point3.h"
#include "geometry/triangle.h"
#include "geometry/vector3.h"

#include <vector>

namespace kd_tree
{
    struct AAB;
}

class Ray {
public:
	Point3 origin;
	Vector3 direction;

	Ray(Point3 &origin, Vector3 &direction);

	// Compute the resulting point of the ray given parameter t
	Point3 at(double t) const;

	// Triangle intersection function
	bool hit(const Triangle &tri, double &t, 
		std::vector<double> &bar_weights) const;

    // AABB intersection function
    bool hit(const kd_tree::AAB &aabb, double &t_near, double &t_far) const;
};

#include "kd-tree/kd_tree.h"

#endif
