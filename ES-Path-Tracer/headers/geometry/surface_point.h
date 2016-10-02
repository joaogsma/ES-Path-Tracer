#ifndef __GUARD_SURFACE_POINT_H__
#define __GUARD_SURFACE_POINT_H__

#include "geometry/point3.h"
#include "geometry/vector3.h"

class Surface_Point : public Point3 {
public:
	const Vector3 normal;

	Surface_Point(double x, double y, double z, const Vector3 &normal_vector);
};

#endif
