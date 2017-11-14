#ifndef ES_PATH_TRACER_GEOMETRY_RAY_H_
#define ES_PATH_TRACER_GEOMETRY_RAY_H_

#include "aab.h"
#include "point3.h"
#include "triangle.h"
#include "vector3.h"

#include <vector>

class Ray {
public:
	Point3 origin;
	Vector3 direction;

	Ray(const Point3 &origin, const Vector3 &direction);

	// Compute the resulting point of the ray given parameter t
	Point3 at(double t) const;

	// Triangle intersection function
	bool intersect(const Triangle &tri, double &t, 
		std::vector<double> &bar_weights) const;

    // AABB intersection function
    bool intersect(const AAB &aabb, double &t_near, double &t_far) const;
};

#endif
