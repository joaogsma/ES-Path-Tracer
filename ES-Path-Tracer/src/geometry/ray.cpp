#include "ray.h"

Ray::Ray(Point3& origin, Vector3& direction) : origin(origin), 
	direction( direction.normalize() ) {}

Point3 Ray::at(double t)
{
	double x_ = origin.x + (t * direction.x);
	double y_ = origin.y + (t * direction.y);
	double z_ = origin.z + (t * direction.z);

	return Point3(x_, y_, z_);
}