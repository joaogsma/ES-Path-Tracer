#include "geometry/point3.h"
#include "geometry/triangle.h"
#include "geometry/vector3.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

using std::copy;
using std::vector;
using std::abs;

Triangle::Triangle(Point3* point1, Point3* point2, Point3* point3,
    Vector3* normal1, Vector3* normal2, Vector3* normal3)
{
	m_vertices[0] = point1;
	m_vertices[1] = point2;
	m_vertices[2] = point3;

    m_normals[0] = normal1;
    m_normals[1] = normal2;
    m_normals[2] = normal3;
}


Triangle::Triangle(const Triangle& other)
{
	copy( other.m_vertices, other.m_vertices + 3, m_vertices );
    copy( other.m_normals, other.m_normals + 3, m_normals );
}


Triangle& Triangle::operator=(const Triangle& other)
{
    if ( &other != this )
    {
        copy( other.m_vertices, other.m_vertices + 3, m_vertices );
        copy( other.m_normals, other.m_normals + 3, m_normals );
    }

	return *this;
}


vector<double> Triangle::baricentric_coordinates(const Point3& p) const
{
	const Point3 &a = *vertex(0);
	const Point3 &b = *vertex(1);
	const Point3 &c = *vertex(2);

	double area_abc = cross_prod( Vector3(a, b), Vector3(a, c) ).magnitude();

	double area_pbc = cross_prod( Vector3(p, b), Vector3(p, c) ).magnitude();
	double alpha = area_pbc / area_abc;

	double area_pca = cross_prod( Vector3(p, c), Vector3(p, a) ).magnitude();
	double beta = area_pca / area_abc;

	double area_pab = cross_prod( Vector3(p, a), Vector3(p, b) ).magnitude();
	double gama = area_pab / area_abc;

	return { alpha, beta, gama };
}
