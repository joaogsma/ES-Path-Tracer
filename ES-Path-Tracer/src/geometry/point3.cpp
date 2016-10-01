#include "point3.h"

#include <math.h>

double distance(const Point3& a, const Point3& b)
{
	double x_diff = a.x - b.x;
	double y_diff = a.y - b.y;
	double z_diff = a.z - b.z;

	return sqrt(pow(x_diff, 2) + pow(y_diff, 2) + pow(z_diff, 2));
}

Point3::Point3(double x, double y, double z) : x(x), y(y), z(z) {}
