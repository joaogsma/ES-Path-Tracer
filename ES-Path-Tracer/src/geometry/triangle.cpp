#include "geometry/point3.h"
#include "geometry/surface_point.h"
#include "geometry/triangle.h"
#include "geometry/vector3.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

using std::copy;
using std::vector;
using std::abs;

Triangle::Triangle(Surface_Point* point1, Surface_Point* point2, Surface_Point* point3)
	: v1(vertices[0]), v2(vertices[1]), v3(vertices[2])
{
	vertices[0] = point1;
	vertices[1] = point2;
	vertices[2] = point3;
}


Triangle::Triangle(const Triangle& other)
	: v1(vertices[0]), v2(vertices[1]), v3(vertices[2])
{
	copy( other.begin(), other.end(), begin() );
}


Triangle& Triangle::operator=(const Triangle& other)
{
	copy(other.begin(), other.end(), begin());
	return *this;
}


vector<double> Triangle::baricentric_coordinates(const Point3& p) const
{
	const Point3 &a = *v1;
	const Point3 &b = *v2;
	const Point3 &c = *v3;

	double area_abc = cross_prod( Vector3(a, b), Vector3(a, c) ).magnitude();

	double area_pbc = cross_prod( Vector3(p, b), Vector3(p, c) ).magnitude();
	double alpha = area_pbc / area_abc;

	double area_pca = cross_prod( Vector3(p, c), Vector3(p, a) ).magnitude();
	double beta = area_pca / area_abc;

	double area_pab = cross_prod( Vector3(p, a), Vector3(p, b) ).magnitude();
	double gama = area_pab / area_abc;

	return { alpha, beta, gama };
}
