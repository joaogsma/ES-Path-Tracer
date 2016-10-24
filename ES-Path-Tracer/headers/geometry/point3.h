#ifndef  __GUARD_POINT3_H__
#define __GUARD_POINT3_H__

#include <algorithm>
#include <vector> 

#include "vector3.h"

class Point3 {
public:
	static const int N_DIMS = 3;
	static const double COMP_EPSILON;

	typedef double* iterator;
	typedef const double* const_iterator;

	double& x;
	double& y;
	double& z;

	Point3(double x, double y, double z);

	// Conversion constructor from Vector3
	Point3(const Vector3& vector);

	// Copy constructor
	Point3(const Point3& other);

	// Coordinate indexing
	double& operator[](int i) { return coordinates[i]; }
	const double& operator[](int i) const { return coordinates[i]; }

	// Iterator functions
	iterator begin() { return coordinates; }
	iterator end() { return coordinates + N_DIMS; }
	const_iterator begin() const { return coordinates; }
	const_iterator end() const { return coordinates + N_DIMS; }

	Point3& operator=(const Point3& other);

	bool operator==(const Point3& other) const;

private:
	double coordinates[N_DIMS];
};

double distance(const Point3& a, const Point3& b);

#endif