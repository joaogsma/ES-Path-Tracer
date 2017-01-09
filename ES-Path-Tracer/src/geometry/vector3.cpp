#include <algorithm>
#include <cstdlib>
#include <math.h>

#include "geometry/vector3.h"
#include "geometry/point3.h"

const double Vector3::COMP_EPSILON = 1E-6;

Vector3::Vector3(double x, double y, double z) : x(coordinates[0]),
y(coordinates[1]), z(coordinates[2])
{
    coordinates[0] = x;
    coordinates[1] = y;
    coordinates[2] = z;
}

Vector3::Vector3(const Point3& origin, const Point3& destination)
    : x(coordinates[0]), y(coordinates[1]), z(coordinates[2])
{
    coordinates[0] = destination.x - origin.x;
    coordinates[1] = destination.y - origin.y;
    coordinates[2] = destination.z - origin.z;
}

Vector3::Vector3(const Point3& point) : x(coordinates[0]), 
y(coordinates[1]), z(coordinates[2])
{
	std::copy(point.begin(), point.end(), begin());
}

// Copy constructor
Vector3::Vector3(const Vector3& other) : x(coordinates[0]), 
	y(coordinates[1]), z(coordinates[2])
{
	double* other_coord = &other.x;
	std::copy(other_coord, other_coord + N_DIMS, coordinates);
}

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

Vector3& Vector3::operator=(const Vector3& other)
{
	if ( &other != this )
		std::copy(other.begin(), other.end(), coordinates);
	
	return *this;
}

bool Vector3::operator==(const Vector3& other) const
{
	if ( &other == this )
		return true;

	double x_diff = x - other.x;
	double y_diff = y - other.y;
	double z_diff = z - other.z;

	return (abs(x_diff) + abs(y_diff) + abs(z_diff)) <= COMP_EPSILON;
}

Vector3 operator*(const Vector3& vec, double scalar)
{
    return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

Vector3 operator*(double& scalar, const Vector3& vec)
{
    return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

Vector3 operator+(const Vector3& vec1, const Vector3& vec2)
{
	return Vector3(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
}

Vector3 operator-(const Vector3& vec1, const Vector3& vec2)
{
	return Vector3(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z);
}

Vector3 operator/(const Vector3& vec, double scalar)
{
    return Vector3(vec.x / scalar, vec.y / scalar, vec.z / scalar);
}