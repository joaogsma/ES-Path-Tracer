#ifndef __GUARD_TRIANGLE_H__
#define __GUARD_TRIANGLE_H__

#include "geometry/point3.h"
#include "geometry/vector3.h"

#include <vector>

/*	Triangle objects correspond to triangles in scene object meshes */
class Triangle {

public:
    Triangle(Point3* point1, Point3* point2, Point3* point3, 
        Vector3* normal1, Vector3* normal2, Vector3* normal3);

	Triangle(const Triangle& other);

	// Accessor functions
    Point3* vertex(int i) const { return m_vertices[i]; };
    Vector3* normal(int i) const { return m_normals[i]; };

	std::vector<double> baricentric_coordinates(const Point3& p) const;

	Triangle& operator=(const Triangle& other);

private:
	Point3* m_vertices[3];
    Vector3* m_normals[3];
};

#endif