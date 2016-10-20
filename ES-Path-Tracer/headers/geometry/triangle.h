#ifndef __GUARD_TRIANGLE_H__
#define __GUARD_TRIANGLE_H__

#include "geometry/point3.h"
#include "geometry/surface_point.h"
#include "geometry/vector3.h"

#include <vector>

/*	Triangle objects correspond to triangles in scene object meshes */
class Triangle {

public:
	static const int N_VERTICES = 3;
	
	typedef Surface_Point** iterator;
	typedef Surface_Point* const * const_iterator;

	Surface_Point*& v1;
	Surface_Point*& v2;
	Surface_Point*& v3;

	Triangle(Surface_Point* point1, Surface_Point* point2, Surface_Point* point3);

	Triangle(const Triangle& other);

	// Iterator functions
	iterator begin() { return &vertices[0]; }
	iterator end() { return &vertices[N_VERTICES]; }
	const_iterator begin() const { return &vertices[0]; }
	const_iterator end() const { return &vertices[N_VERTICES]; }

	// Vertex indexing
	Surface_Point*& operator[](int i) { return vertices[i]; }
	Surface_Point* const & operator[](int i) const { return vertices[i]; }

	std::vector<double> baricentric_coordinates(const Point3& p) const;

	Triangle& operator=(const Triangle& other);

private:
	Surface_Point* vertices[N_VERTICES];
};

#endif