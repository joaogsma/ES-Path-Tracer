#ifndef __GUARD_POINT3_H__
#define __GUARD_POINT3_H__

#include <algorithm>
#include <cassert>
#include <numeric>
#include <string>
#include <vector>

class Vector3;

class Point3 {
public:
	static const int N_DIMS = 3;
	static const double COMP_EPSILON;

	typedef double* iterator;
	typedef const double* const_iterator;

	double& x;
	double& y;
	double& z;

    // Constructors based on coordinates
    Point3(double val);
	Point3(double x, double y, double z);

	// Conversion constructor from Vector3
	explicit Point3(const Vector3& vector);

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
    bool operator!=(const Point3& other) const { return !operator==(other); }
    void operator+=(const Vector3& vec);
    void operator-=(const Vector3& vec);

	std::string to_string() const
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	}

private:
	double coordinates[N_DIMS];
};

double distance(const Point3& a, const Point3& b);
double distance2(const Point3& a, const Point3& b);

Vector3 operator-(const Point3& a, const Point3& b);
Point3 operator+(const Point3& p, const Vector3& v);
Point3 operator+(const Vector3& v, const Point3& p);

template <class In_Iterator1, class In_Iterator2>
Point3 affine_combination(In_Iterator1 weights_begin, In_Iterator1 weights_end,
    In_Iterator2 points_begin, In_Iterator2 points_end)
{
    static_assert( std::is_floating_point<typename std::iterator_traits<In_Iterator1>::value_type>::value, "Not floating point weights" );
    static_assert( std::is_same<Point3, typename std::iterator_traits<In_Iterator2>::value_type>::value, "Not Point3 elements" );
    assert( weights_end - weights_begin == points_end - points_begin );
    assert( std::accumulate(weights_begin, weights_end, 0.0) == 1.0 );

    const Point3& initial_point = *points_begin;
    Point3 acc = *points_begin;

    ++weights_begin;
    ++points_begin;

    while (weights_begin != weights_end)
    {
        acc += (*weights_begin) * (*points_begin - initial_point);

        ++weights_begin;
        ++points_begin;
    }

    return acc;
}

// At the end of the header in order to avoid cyclic dependency
#include "vector3.h"

#endif