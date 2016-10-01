#include "point3.h"
#include "surface_point.h"
#include "triangle.h"
#include "vector3.h"

#include <cmath>
#include <vector>

using std::vector;
using std::abs;

Triangle::Triangle(const Surface_Point& point1, const Surface_Point& point2, 
	const Surface_Point& point3) : v1(point1), v2(point2), v3(point3) {}


vector<double> Triangle::baricentric_coordinates(const Point3& p) const
{
	const Point3 &a = v1;
	const Point3 &b = v2;
	const Point3 &c = v3;

	double area_abc = cross_prod( Vector3(a, b), Vector3(a, c) ).magnitude();

	double area_pbc = cross_prod( Vector3(p, b), Vector3(p, c) ).magnitude();
	double alpha = area_pbc / area_abc;

	double area_pca = cross_prod( Vector3(p, c), Vector3(p, a) ).magnitude();
	double beta = area_pca / area_abc;

	double area_pab = cross_prod( Vector3(p, a), Vector3(p, b) ).magnitude();
	double gama = area_pab / area_abc;

	return { alpha, beta, gama };
}


bool Triangle::intersect(const Ray& ray, double& t, vector<double>& weights) const
{
	Vector3 e1 (v1, v2);
	Vector3 e2 (v1, v3);
	Vector3 q = cross_prod(ray.direction, e2);

	double a = dot_prod(e1, q);

	const Vector3 s (v1, ray.origin);
	const Vector3 &r = cross_prod(s, e1);

	// Barycentric vertex weights
	weights.resize(3);
	weights[1] = dot_prod(s, q) / a;
	weights[2] = dot_prod(ray.direction, r) / a;
	weights[0] = 1 - (weights[1] + weights[2]);

	const double dist = dot_prod(e2, r) / a;

	static const double epsilon = 1e-7f;
	static const double epsilon2 = 1e-10;

	if (abs(a) <= epsilon || weights[0] < -epsilon2 || weights[1] < -epsilon2 || 
		weights[2] < -epsilon2 || dist <= 0) 
	{
		// The ray is nearly parallel to the triangle, or the intersection lies 
		// outside the triangle or behind the ray origin: intersection is null
		return false;
	}
	
	t = dist;
	return true;
}
