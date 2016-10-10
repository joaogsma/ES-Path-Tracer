#ifndef __GUARD_SURFACE_POINT_H__
#define __GUARD_SURFACE_POINT_H__

#include "geometry/point3.h"
#include "geometry/vector3.h"

/*	Surface_Points correspond to points in the surface of object meshes (for 
	vertices, see the Vertex class). They are Point3 objects, with the addition
	of having a normal vector. */
class Surface_Point : public Point3 {
public:
	const Vector3 normal;

	Surface_Point(double x, double y, double z, const Vector3 &normal_vector);
};

#endif
