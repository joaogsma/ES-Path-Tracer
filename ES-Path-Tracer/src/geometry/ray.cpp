#include "geometry/ray.h"
#include "geometry/triangle.h"
#include "geometry/vector3.h"

#include <vector>

using std::vector;

// ============================================================================
// =============================== CONSTRUCTOR ================================
// ============================================================================

Ray::Ray(Point3& origin, Vector3& direction) : origin(origin), 
	direction( direction.normalize() ) {}

// ============================================================================



// ============================================================================
// =========================== AUXILIARY FUNCTIONS ============================
// ============================================================================

Point3 Ray::at(double t) const
{
	double x_ = origin.x + (t * direction.x);
	double y_ = origin.y + (t * direction.y);
	double z_ = origin.z + (t * direction.z);

	return Point3(x_, y_, z_);
}

vector<Point3> box_top_vertices(const Point3 &center, const double x_dims,
	const double y_dims, const double z_dims)
{
	double x_off = x_dims / 2;
	double y_off = y_dims / 2;
	double z_off = z_dims / 2;

	vector<Point3> top_vertices;

	top_vertices.push_back(
		Point3(center.x + x_off, center.y + y_off, center.z + z_off));
	top_vertices.push_back(
		Point3(center.x + x_off, center.y + y_off, center.z - z_off));
	top_vertices.push_back(
		Point3(center.x - x_off, center.y + y_off, center.z - z_off));
	top_vertices.push_back(
		Point3(center.x - x_off, center.y + y_off, center.z + z_off));

	return top_vertices;
}

vector<Point3> box_bot_vertices(const Point3 &center, const double x_dims,
	const double y_dims, const double z_dims)
{
	double x_off = x_dims / 2;
	double y_off = y_dims / 2;
	double z_off = z_dims / 2;

	vector<Point3> bot_vertices;

	bot_vertices.push_back(
		Point3(center.x + x_off, center.y - y_off, center.z + z_off));
	bot_vertices.push_back(
		Point3(center.x + x_off, center.y - y_off, center.z - z_off));
	bot_vertices.push_back(
		Point3(center.x - x_off, center.y - y_off, center.z - z_off));
	bot_vertices.push_back(
		Point3(center.x - x_off, center.y - y_off, center.z + z_off));

	return bot_vertices;
}

// ============================================================================



// ============================================================================
// ========================== INTERSECTION FUNCTIONS ==========================
// ============================================================================

bool Ray::hit(const Triangle &tri, double &t, vector<double> &bar_weights) const
{
	const Point3 &v1 = *tri.v1;
	const Point3 &v2 = *tri.v2;
	const Point3 &v3 = *tri.v3;
	
	Vector3 e1(v1, v2);
	Vector3 e2(v1, v3);
	Vector3 q = cross_prod(direction, e2);

	double a = dot_prod(e1, q);

	const Vector3 s(v1, origin);
	const Vector3 &r = cross_prod(s, e1);

	// Barycentric vertex weights
	bar_weights.resize(3);
	bar_weights[1] = dot_prod(s, q) / a;
	bar_weights[2] = dot_prod(direction, r) / a;
	bar_weights[0] = 1 - (bar_weights[1] + bar_weights[2]);

	const double dist = dot_prod(e2, r) / a;

	static const double epsilon = 10e-7f;
	static const double epsilon2 = 10e-10;

	if ( abs(a) <= epsilon || bar_weights[0] < -epsilon2 || 
		bar_weights[1] < -epsilon2 || bar_weights[2] < -epsilon2 || 
		dist <= 0 )
	{
		// The ray is nearly parallel to the triangle, or the intersection lies 
		// outside the triangle or behind the ray origin: intersection is null
		return false;
	}

	t = dist;
	return true;
}

// ============================================================================