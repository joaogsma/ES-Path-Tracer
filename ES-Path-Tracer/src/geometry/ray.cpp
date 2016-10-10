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

	static const double epsilon = 1e-7f;
	static const double epsilon2 = 1e-10;

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

bool Ray::hit(const Region &kd_region) const
{
	// If the origin is inside the region, return true
	if (kd_region.min_x <= origin.x && origin.x <= kd_region.max_x &&
		kd_region.min_y <= origin.y && origin.y <= kd_region.max_y &&
		kd_region.min_z <= origin.z && origin.z <= kd_region.max_z)
	{
		return true;
	}

	// Compute the region's center point
	const Point3 center( (kd_region.max_x + kd_region.min_x) / 2, 
		(kd_region.max_y + kd_region.min_y) / 2, 
		(kd_region.max_z + kd_region.min_z) / 2 );

	// Computet he region's dimensions in each axis
	const double x_dims = kd_region.max_x - kd_region.min_x;
	const double y_dims = kd_region.max_y - kd_region.min_y;
	const double z_dims = kd_region.max_z - kd_region.min_z;

	// Compute the region's vertices
	vector<Point3> top_vertices = box_top_vertices(center, x_dims, y_dims, z_dims);
	vector<Point3> bot_vertices = box_bot_vertices(center, x_dims, y_dims, z_dims);

	// Check the 8 side triangles for intersections
	for (vector<Point3>::size_type i = 0; i < 4; i++)
	{
		// ===== Create the triangle vertices =====
		Vertex v1( top_vertices[i].x, top_vertices[i].y, top_vertices[i].z, 
			Vector3(0, 0, 0) );
		
		Vertex v2( bot_vertices[i].x, bot_vertices[i].y, bot_vertices[i].z, 
			Vector3(0, 0, 0) );
		
		vector<Point3>::size_type next_i = (i + 1) % 4;
		
		Vertex v3( bot_vertices[next_i].x, bot_vertices[next_i].y, 
			bot_vertices[next_i].z, Vector3(0, 0, 0) );
		
		Vertex v4( top_vertices[next_i].x, top_vertices[next_i].y, 
			top_vertices[next_i].z, Vector3(0, 0, 0) );
		// ========================================

		// ===== Check for intersections =====
		double t;
		vector<double> weights;

		Triangle tri1(&v1, &v2, &v3);
		if (hit(tri1, t, weights))
			return true;
		
		Triangle tri2(&v3, &v4, &v1);
		if (hit(tri2, t, weights))
			return true;
		// ===================================
	}


	// ========== Check the 2 top triangles for intersections ==========
	{
		Vertex v1( top_vertices[0].x, top_vertices[0].y, top_vertices[0].z, 
			Vector3(0, 0, 0) );

		Vertex v2( top_vertices[1].x, top_vertices[1].y, top_vertices[1].z, 
			Vector3(0, 0, 0) );

		Vertex v3( top_vertices[2].x, top_vertices[2].y, top_vertices[2].z, 
			Vector3(0, 0, 0) );

		Vertex v4( top_vertices[3].x, top_vertices[3].y, top_vertices[3].z, 
			Vector3(0, 0, 0) );

		double t;
		vector<double> weights;

		Triangle tri1(&v1, &v2, &v3);
		if (hit(tri1, t, weights))
			return true;

		Triangle tri2(&v3, &v4, &v1);
		if (hit(tri2, t, weights))
			return true;
	}
	// =================================================================


	// ========== Check the 2 bottom triangles for intersections ==========
	{
		Vertex v1( bot_vertices[0].x, bot_vertices[0].y, bot_vertices[0].z, 
			Vector3(0, 0, 0) );

		Vertex v2( bot_vertices[1].x, bot_vertices[1].y, bot_vertices[1].z, 
			Vector3(0, 0, 0) );

		Vertex v3( bot_vertices[2].x, bot_vertices[2].y, bot_vertices[2].z, 
			Vector3(0, 0, 0) );

		Vertex v4( bot_vertices[3].x, bot_vertices[3].y, bot_vertices[3].z, 
			Vector3(0, 0, 0) );

		double t;
		vector<double> weights;

		Triangle tri1(&v1, &v2, &v3);
		if (hit(tri1, t, weights))
			return true;

		Triangle tri2(&v3, &v4, &v1);
		if (hit(tri2, t, weights))
			return true;
	}
	// ====================================================================

	return false;
}

// ============================================================================