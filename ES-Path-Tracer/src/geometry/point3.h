#ifndef  __GUARD_POINT3_H__
#define __GUARD_POINT3_H__

#include <vector>

class Point3;

double distance(const Point3& a, const Point3& b);

class Point3 {
public:
	double x;
	double y;
	double z;

	Point3(double x, double y, double z);
};

#endif