#include <algorithm>
#include <math.h>

#include "geometry/point3.h"
#include "geometry/vector3.h"

using std::copy;

const double Point3::COMP_EPSILON = 10E-9;

// Point constructor
Point3::Point3(double x, double y, double z) : x(coordinates[0]), 
	y(coordinates[1]), z(coordinates[2]) 
{
	coordinates[0] = x;
	coordinates[1] = y;
	coordinates[2] = z;
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
	double x_diff = a.x - b.x;
	double y_diff = a.y - b.y;
	double z_diff = a.z - b.z;

	return sqrt(pow(x_diff, 2) + pow(y_diff, 2) + pow(z_diff, 2));
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