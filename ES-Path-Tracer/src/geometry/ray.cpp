#include "geometry/ray.h"
#include "geometry/triangle.h"
#include "geometry/vector3.h"

#include <algorithm>
#include <cmath>
#include <vector>

#define HANDLE_NAN true

using std::vector;

// ============================================================================
// =============================== CONSTRUCTOR ================================
// ============================================================================

Ray::Ray(const Point3& origin, const Vector3& direction) : origin(origin), 
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

// ============================================================================



// ============================================================================
// ========================== INTERSECTION FUNCTIONS ==========================
// ============================================================================

bool Ray::intersect(const Triangle &tri, double &t, vector<double> &bar_weights) const
{
	const Point3 &v0 = *tri.vertex(0);
	const Point3 &v1 = *tri.vertex(1);
	const Point3 &v2 = *tri.vertex(2);
	
	Vector3 e1(v0, v1);
	Vector3 e2(v0, v2);
	Vector3 q = cross_prod(direction, e2);

	double a = dot_prod(e1, q);

	const Vector3 s(v0, origin);
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

bool Ray::intersect(const AAB &aabb, double &t_near, double &t_far) const
{
    /*  Compute the inverse of the ray normalize in each axis. Used as denominator 
        and for sign checks on the ray normalize */
    double invdir_x = 1 / direction.x;
    double invdir_y = 1 / direction.y;
    double invdir_z = 1 / direction.z;

    double tmin, tmax;

    // X axis
    double tx1 = (aabb.max_x - origin.x) * invdir_x;
    double tx2 = (aabb.min_x - origin.x) * invdir_x;

    tmin = std::min(tx1, tx2);
    tmax = std::max(tx1, tx2);

    // Y axis
    double ty1 = (aabb.max_y - origin.y) * invdir_y;
    double ty2 = (aabb.min_y - origin.y) * invdir_y;

    if (HANDLE_NAN)
    {
        tmin = std::max(tmin, std::min(std::min(ty1, ty2), tmax));
        tmax = std::min(tmax, std::max(std::max(ty1, ty2), tmin));
    }
    else
    {
        tmin = std::max(tmin, std::min(ty1, ty2));
        tmax = std::min(tmax, std::max(ty1, ty2));
    }

    // Z axis
    double tz1 = (aabb.max_z - origin.z) * invdir_z;
    double tz2 = (aabb.min_z - origin.z) * invdir_z;

    if (HANDLE_NAN)
    {
        tmax = std::min(tmax, std::max(std::max(tz1, tz2), tmin));
        tmin = std::max(tmin, std::min(std::min(tz1, tz2), tmax));
    }
    else
    {
        tmin = std::max(tmin, std::min(tz1, tz2));
        tmax = std::min(tmax, std::max(tz1, tz2));
    }

    if (tmax >= std::max(tmin, 0.0))
    {
        t_near = tmin;
        t_far = tmax;
    
        return true;
    }
    
    return false;
}

// ============================================================================