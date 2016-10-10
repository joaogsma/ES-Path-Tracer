#ifndef __GUARD_VECTOR3_H__
#define __GUARD_VECTOR3_H__

#include <vector>

#include "geometry/point3.h"

class Vector3;

Vector3 cross_prod(const Vector3& a, const Vector3& b);

double dot_prod(const Vector3& a, const Vector3& b);

class Vector3 {
public:
    double x;
    double y;
    double z;

    Vector3(double x, double y, double z);

	Vector3(const Point3& origin, const Point3& destination);
	
	// Returns a new Vector3 object corresponding to the normalized vector
	Vector3 normalize() const;

	double magnitude() const;

};

#endif