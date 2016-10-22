#include "geometry/point3.h"
#include "geometry/surface_point.h"

Surface_Point::Surface_Point(double x, double y, double z) : Point3(x, y, z), normal (0, 0, 0) {} 


Surface_Point::Surface_Point(const Point3& point) : Point3(point), normal(0, 0, 0) {}


Surface_Point::Surface_Point(double x, double y, double z, 
	const Vector3 &normal_vector) : Point3(x, y, z), 
	normal (normal_vector.normalize() ) {}
