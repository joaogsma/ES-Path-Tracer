#include "geometry/point3.h"
#include "geometry/surface_point.h"
#include "geometry/triangle.h"
#include "geometry/vector3.h"

#include <cmath>
#include <memory>
#include <vector>

using std::vector;
using std::abs;
using std::shared_ptr;

Triangle::Triangle(const shared_ptr<Surface_Point> &point1,
	const shared_ptr<Surface_Point> &point2,
	const shared_ptr<Surface_Point> &point3) :
	v1(point1), v2(point2), v3(point3) {}


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
