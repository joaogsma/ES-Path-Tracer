#include "geometry/point3.h"
#include "geometry/ray.h"
#include "geometry/triangle.h"
#include "geometry/vector3.h"
#include "kd-tree/kd_tree.h"
#include "evolution-strategy/individual.h"

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

static Ray ray(Point3(5, 1, 0.1), Vector3(-1, 0, (-0.1/4)));

bool contains_region(const kd_tree::Region& kd_region)
{
	return ray.contains_region(kd_region);
}

bool hit(const Triangle& tri)
{
	double t;
	vector<double> weights;
	return ray.hit(tri, t, weights);
}

bool hit(const kd_tree::Region& kd_region)
{
	return ray.hit(kd_region);
}

int main()
{
	Surface_Point p1(0, 0, 0, Vector3(0, 0, 1));
	Surface_Point p2(1, 0, 0, Vector3(0, 0, 1));
	Surface_Point p3(1, 1, 0, Vector3(0, 0, 1));
	Surface_Point p4(0, 1, 0, Vector3(0, 0, 1) );
	
	Triangle t1(&p1, &p2, &p3);
	Triangle t2(&p3, &p4, &p1);

	vector<const Triangle*> triangles = { &t1, &t2 };
	
	kd_tree::KD_Tree* tree = kd_tree::KD_Tree::build_tree(triangles.begin(), triangles.end());

	vector<const Triangle*> intersected;
	tree->search(intersected, contains_region, hit, hit);

	delete tree;

	return 0;

}