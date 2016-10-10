#include "geometry/point3.h"
#include "geometry/surface_point.h"
#include "geometry/triangle.h"
#include "geometry/vector3.h"

#include <cmath>
#include <vector>

using std::vector;
using std::abs;

Triangle::Triangle(Vertex * const point1, Vertex * const point2, 
	Vertex * const point3) : v1(point1), v2(point2), v3(point3) 
{
	// Increase each vertex's triangle counter
	v1->triangle_cout++;
	v2->triangle_cout++;
	v3->triangle_cout++;
}

Triangle::~Triangle()
{
	/*	For each Vertex, decrease the counter of triangles pointing to it.
		Also, if this was the last such triangle, delete the vertex object. */

	if ( --(v1->triangle_cout) == 0 )
		delete v1;

	if ( --(v2->triangle_cout) == 0 )
		delete v2;

	if ( --(v3->triangle_cout) == 0 )
		delete v3;
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
