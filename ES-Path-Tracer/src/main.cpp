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

int main()
{
	Point3 p1(0, 0, 0);
	Point3 p2(1, 0, 0);
	Point3 p3(1, 1, 0);
	Point3 p4(0, 1, 0);
	
	Triangle t1(&p1, &p2, &p3);
	Triangle t2(&p3, &p4, &p1);

	vector<const Triangle*> triangles = { &t1, &t2 };
	
	kd_tree::KD_Tree tree(triangles);

	vector<const Triangle*> intersected;
	
	return 0;

}