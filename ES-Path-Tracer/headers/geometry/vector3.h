#ifndef ES_PATH_TRACER_GEOMETRY_VECTOR3_H_
#define ES_PATH_TRACER_GEOMETRY_VECTOR3_H_

#include <algorithm>
#include <vector>
#include <string>

#include "point3.h"

class Vector3 {
public:

	typedef double* iterator;
	typedef const double* const_iterator;

	double& x;
    double& y;
    double& z;

    // Constructors based on coordinates
    Vector3(double val);
    Vector3(double x, double y, double z);

	// Constructor based on initial and end points
	Vector3(const Point3& origin, const Point3& destination);

	// Conversion constructor from Point3
	explicit Vector3(const Point3& point);

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
    bool operator!=(const Vector3& other) const { return !operator==(other); }

    void operator+=(const Vector3& other);
    void operator-=(const Vector3& other);
	
	void operator*=(double scalar);
	void operator/=(double scalar);

	// Returns a new Vector3 object corresponding to the normalized vector
	Vector3 normalize() const;

	double magnitude() const;

	std::string to_string() const
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	}

private:
	static const int N_DIMS = 3;
	static const double COMP_EPSILON;
	double coordinates[N_DIMS];
};

// Cross and dot products
Vector3 cross_prod(const Vector3& a, const Vector3& b);
double dot_prod(const Vector3& a, const Vector3& b);

// Vector multiplication by a scalar
Vector3 operator*(const Vector3& vec, double scalar);
Vector3 operator*(double scalar, const Vector3& vec);

// Vector division by a scalar
Vector3 operator/(const Vector3& vec, double scalar);

// Vector sum
Vector3 operator+(const Vector3& vec1, const Vector3& vec2);

// Vector subtraction
Vector3 operator-(const Vector3& vec1, const Vector3& vec2);

#endif
