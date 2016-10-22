#ifndef __GUARD_VECTOR3_H__
#define __GUARD_VECTOR3_H__

#define _SCL_SECURE_NO_WARNINGS

#include <algorithm>
#include <vector>

#include "point3.h"

class Vector3 {
public:
	static const int N_DIMS = 3;
	const double COMP_EPSILON = 10E-9;

	typedef double* iterator;
	typedef const double* const_iterator;

	double& x;
    double& y;
    double& z;

	// Constructor based on coordinates
    Vector3(double x, double y, double z);

	// Constructor based on initial and end points
	Vector3(const Point3& origin, const Point3& destination);

	// Conversion constructor from Point3
	Vector3(const Point3& point);

	// Copy constructor
	Vector3(const Vector3& other);
	
	// Coordinate indexing
	double& operator[](int i) { return coordinates[i]; }
	const double& operator[](int i) const { return coordinates[i]; }

	// Iterator functions
	iterator begin() { return coordinates; }
	iterator end() { return coordinates + N_DIMS; }
	const_iterator begin() const { return coordinates; }
	const_iterator end() const { return coordinates + N_DIMS; }

	Vector3& operator=(const Vector3& other);
	
	bool operator==(const Vector3& other) const;

	// Returns a new Vector3 object corresponding to the normalized vector
	Vector3 normalize() const;

	double magnitude() const;

private:
	double coordinates[N_DIMS];

};


// Cross and dot products
Vector3 cross_prod(const Vector3& a, const Vector3& b);
double dot_prod(const Vector3& a, const Vector3& b);

// Vector multiplication by a scalar
template <class T> Vector3 operator*(const Vector3& vec, const T& scalar);
template <class T> Vector3 operator*(const T& scalar, const Vector3& vec);

// Vector sum
Vector3 operator+(const Vector3& vec1, const Vector3& vec2);

// Vector subtraction
Vector3 operator-(const Vector3& vec1, const Vector3& vec2);

// Vector division
template <class T> Vector3 operator/(const Vector3& vec, const T& scalar);

#endif