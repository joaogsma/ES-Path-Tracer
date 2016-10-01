#ifndef __GUARD_RAY_H__
#define __GUARD_RAY_H__

#include "point3.h"
#include "vector3.h"

class Ray {
public:
	const Point3 origin;
	const Vector3 direction;

	Ray(Point3& origin, Vector3& direction);

	Point3 at(double t);
};

#endif
