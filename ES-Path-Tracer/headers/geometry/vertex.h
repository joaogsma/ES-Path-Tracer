#ifndef __GUARD_VERTEX_H__
#define __GUARD_VERTEX_H__

#include "geometry/surface_point.h"
#include "geometry/vector3.h"

/*	Objects of the Vertex class correspond to the vertices in the triangle mesh. 
	They are essentially Surface_Points, but with a counter to the number of
	triangles to which they belong. This is used when the triangles are
	deleted, so that the last triangle can delete the Vertex object. */
class Vertex : public Surface_Point {
public:
	unsigned int triangle_cout;
	
	Vertex(double x, double y, double z, const Vector3 &normal);
};

#endif
