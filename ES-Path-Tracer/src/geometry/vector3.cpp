#include "vector3.h"
#include <math.h>

using std::sqrt;
using std::pow;

Vector3 cross_prod(const Vector3& a, const Vector3& b)
{
	double x_ = (a.y * b.z) - (a.z * b.y);
	double y_ = (a.z * b.x) - (a.x * b.z);
	double z_ = (a.x * b.y) - (a.y * b.x);

	return Vector3(x_, y_, z_);
}


double dot_prod(const Vector3& a, const Vector3& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}


Vector3::Vector3(double x, double y, double z) : x(x), y(y), z(z) {}


Vector3::Vector3(const Point3& origin, const Point3& destination) : x( destination.x - origin.x ), 
	y( destination.y - origin.y ), z( destination.z - origin.z ) {}


Vector3 Vector3::normalize() const
{
	double mag = magnitude();
	
	// Normalization is not defined for the zero vector
	const double epsilon2 = 1e-10;
	if (mag < epsilon2)
		return Vector3(x, y, z);

	return Vector3(x / mag, y / mag, z / mag);
}

double Vector3::magnitude() const
{
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}
