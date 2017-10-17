#include <algorithm>
#include <math.h>

#include "geometry/point3.h"
#include "geometry/vector3.h"

using std::copy;

const double Point3::COMP_EPSILON = 10E-9;

// Point3 constructor
Point3::Point3(double val) : x(coordinates[0]),
    y(coordinates[1]), z(coordinates[2])
{
    x = y = z = val;
}

// Point3 constructor
Point3::Point3(double _x, double _y, double _z) : x(coordinates[0]), 
	y(coordinates[1]), z(coordinates[2]) 
{
	coordinates[0] = _x;
	coordinates[1] = _y;
	coordinates[2] = _z;
}


Point3::Point3(const Vector3& vector) : x(coordinates[0]), 
	y(coordinates[1]), z(coordinates[2])
{
	copy(vector.begin(), vector.end(), begin());
}


// Copy constructor
Point3::Point3(const Point3& other) : x(coordinates[0]),
y(coordinates[1]), z(coordinates[2])
{
	double* other_coord = &other.x;
	copy(other_coord, other_coord + N_DIMS, begin());
}


// Euclidean point distance
double distance(const Point3& a, const Point3& b)
{
	return distance2(a, b);
}

// Euclidean squared point distance
double distance2(const Point3& a, const Point3& b)
{
	double x_diff = a.x - b.x;
	double y_diff = a.y - b.y;
	double z_diff = a.z - b.z;

	return pow(x_diff, 2) + pow(y_diff, 2) + pow(z_diff, 2);
}


Point3& Point3::operator=(const Point3& other)
{
	if ( &other != this )
		copy(other.begin(), other.end(), begin());
	
	return *this;
}


bool Point3::operator==(const Point3& other) const
{
	if ( &other == this )
		return true;

	double x_diff = x - other.x;
	double y_diff = y - other.y;
	double z_diff = z - other.z;

	return ( abs(x_diff) + abs(y_diff) + abs(z_diff) ) <= COMP_EPSILON;
}


void Point3::operator+=(const Vector3& vec)
{
    x += vec.x;
    y += vec.y;
    z += vec.z;
}


void Point3::operator-=(const Vector3& vec)
{
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
}


Vector3 operator-(const Point3& a, const Point3& b)
{
    return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}


Point3 operator+(const Point3& p, const Vector3& v)
{
    return Point3(p.x + v.x, p.y + v.y, p.z + v.z);
}


Point3 operator+(const Vector3& v, const Point3& p)
{
    return Point3(p.x + v.x, p.y + v.y, p.z + v.z);
}
