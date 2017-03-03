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

int main()
{
    Ray r(Point3(1, 1, 1), Vector3(1, 1, 1));
	
    kd_tree::AAB region;
    region.min_x = region.min_y = region.min_z = -1;
    region.max_x = region.max_y = region.max_z = 1;

    double tn, tf;
    if (r.hit(region, tn, tf))
        std::cout << "hit with t = " << tn << ", " << tf << std::endl;
    else
        std::cout << "miss" << std::endl;

	return 0;

}