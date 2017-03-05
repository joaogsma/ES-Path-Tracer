#ifndef __GUARD_RAY_H__
#define __GUARD_RAY_H__

#include "aab.h"
#include "point3.h"
#include "triangle.h"
#include "vector3.h"

#include <vector>

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
    bool hit(const AAB &aabb, double &t_near, double &t_far) const;
};

#include "kd-tree/kd_tree.h"

#endif
