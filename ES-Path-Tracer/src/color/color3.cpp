#include <algorithm>
#include <cmath>

#include "color/color3.h"

using std::copy;
using std::abs;

static const double epsilon = 10E-9;

// Color constructor
Color3::Color3(double red, double green, double blue) : r(coordinates[0]),
g(coordinates[1]), b(coordinates[2])
{
    coordinates[0] = r;
    coordinates[1] = g;
    coordinates[2] = b;
}


// Copy constructor
Color3::Color3(const Color3& other) : r(coordinates[0]),
g(coordinates[1]), b(coordinates[2])
{
    double* other_coord = &other.r;
    copy(other_coord, other_coord + 3, begin());
}


Color3& Color3::operator=(const Color3& other)
{
    if (&other != this)
        copy(other.begin(), other.end(), begin());

    return *this;
}


bool Color3::operator==(const Color3& other) const
{
    if (&other == this)
        return true;

    double r_diff = r - other.r;
    double g_diff = g - other.g;
    double b_diff = b - other.b;

    return (abs(r_diff) + abs(g_diff) + abs(b_diff)) <= epsilon;
}