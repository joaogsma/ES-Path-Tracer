#ifndef __GUARD_TRIANGLE_H__
#define __GUARD_TRIANGLE_H__

#include "geometry/point3.h"
#include "geometry/surface_point.h"
#include "geometry/vector3.h"
#include "geometry/vertex.h"

#include <vector>

/*	Triangle objects correspond to triangles in scene object meshes.
	They contain pointers to their vertices (for memory efficiency). */
class Triangle {
public:
	Vertex * const v1, * const v2, * const v3;

	Triangle(Vertex* point1, Vertex* point2, Vertex* point3);

	~Triangle();

	std::vector<double> baricentric_coordinates(const Point3& p) const;
};

#endif