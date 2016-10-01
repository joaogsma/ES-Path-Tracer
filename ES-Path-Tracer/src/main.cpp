#include "geometry/point3.h"
#include "geometry/ray.h"
#include "geometry/triangle.h"

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
	Surface_Point p1( 0, 1, 0, Vector3(0, 0, 1) );
	Surface_Point p2( -1, 0, 0, Vector3(0, 0, 1) );
	Surface_Point p3( 1, 0, 0, Vector3(0, 0, 1) );

	Ray ray(Point3(0, 0.5, 5), Vector3(0, 0, -1) );

	Triangle tri(p1, p2, p3);
	
	double t;
	vector<double> weights;

	if (tri.intersect(ray, t, weights))
	{
		cout << "intersection point is " << point_to_string( ray.at(t) ) << endl;
	}
	else
	{
		cout << "oops..." << endl;
	}

	system("pause");

	return 0;
}