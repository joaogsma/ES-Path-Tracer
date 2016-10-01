#include "vector3.h"
#include <math.h>

using std::sqrt;
using std::pow;

Vector3::Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

Vector3 Vector3::cross(Vector3& other) {
	double x_ = (this->y * other.z) - (this->z * other.y);
	double y_ = (this->z * other.x) - (this->x * other.z);
	double z_ = (this->x * other.y) - (this->y * other.x);

	return Vector3(x_, y_, z_);
}

double Vector3::dot(Vector3& other) {
	return (this->x * other.x) + (this->y * other.y) + (this->z + other.z);
}

Vector3 Vector3::normalize() {
	double length = sqrt( pow(x, 2) + pow(y, 2) + pow(z, 2) );
	
	// Normalization is not defined for the zero vector
	if (length == 0) return;

	x /= length;
	y /= length;
	z /= length;

	return Vector3(x, y, z);
}

