#include "geometry/point3.h"
#include "geometry/ray.h"
#include "geometry/triangle.h"
#include "geometry/vector3.h"
#include "geometry/vertex.h"
#include "kd-tree/kd_tree.h"

#include <iostream>
#include <sstream>

using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::vector;

string point_to_string(Point3 p)
{
	stringstream ss;
	ss << "(" << p.x << ", " << p.y << ", " << p.z << ")";
	return ss.str();
}

string vector_to_string(Vector3 v)
{
	stringstream ss;
	ss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return ss.str();
}

static Ray ray(Point3(1, 1, 5), Vector3(0, 0, -1));

bool contains_region(const Region& kd_region)
{
	return ray.contains_region(kd_region);
}

bool hit(const Triangle& tri)
{
	double t;
	vector<double> weights;
	return ray.hit(tri, t, weights);
}

bool hit(const Region& kd_region)
{
	return ray.hit(kd_region);
}

int main()
{
	Vertex* p1 = new Vertex(0, 0, 0, Vector3(0, 0, 1) );
	Vertex* p2 = new Vertex(1, 0, 0, Vector3(0, 0, 1) );
	Vertex* p3 = new Vertex(1, 1, 0, Vector3(0, 0, 1) );
	Vertex* p4 = new Vertex(0, 1, 0, Vector3(0, 0, 1) );
	
	Triangle* t1 = new Triangle(p1, p2, p3);
	Triangle* t2 = new Triangle(p3, p4, p1);

	vector<const Triangle*> triangles = { t1, t2 };

	KD_Tree* tree = KD_Tree::build_tree(triangles);

	vector<const Triangle*> intersected;
	tree->search(intersected, contains_region, hit, hit);

	delete tree;

	return 0;

}