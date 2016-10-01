#include "surface_point.h"

Surface_Point::Surface_Point(double x, double y, double z, 
	const Vector3 &normal_vector) : Point3(x, y, z), 
	normal (normal_vector.normalize() ) {}
