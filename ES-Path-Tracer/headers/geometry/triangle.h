#ifndef __GUARD_TRIANGLE_H__
#define __GUARD_TRIANGLE_H__

#include "geometry/point3.h"
#include "geometry/vector3.h"

#include <vector>

/*	Triangle objects correspond to triangles in scene object meshes */
class Triangle {

public:
	typedef Point3** iterator;
	typedef Point3* const * const_iterator;

	Point3*& v1;
	Point3*& v2;
	Point3*& v3;

	Triangle(Point3* point1, Point3* point2, Point3* point3);

	Triangle(const Triangle& other);

	// Iterator functions
	iterator begin() { return &vertices[0]; }
	iterator end() { return &vertices[3]; }
	const_iterator begin() const { return &vertices[0]; }
	const_iterator end() const { return &vertices[3]; }

	// Vertex indexing
	Point3*& operator[](int i) { return vertices[i]; }
	Point3* const & operator[](int i) const { return vertices[i]; }

	std::vector<double> baricentric_coordinates(const Point3& p) const;

	Triangle& operator=(const Triangle& other);

private:
	Point3* vertices[3];
};

#endif