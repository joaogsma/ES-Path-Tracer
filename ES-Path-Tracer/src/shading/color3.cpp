#include <algorithm>
#include <cmath>

#include "shading/color3.h"

using std::copy;
using std::abs;

static const double epsilon = 1e-7;

// Color3 constructor
Color3::Color3(double val) : r(coordinates[0]), g(coordinates[1]), b(coordinates[2])
{
    r = g = b = val;
}

// Color3 constructor
Color3::Color3(double red, double green, double blue) : r(coordinates[0]),
g(coordinates[1]), b(coordinates[2])
{
    coordinates[0] = red;
    coordinates[1] = green;
    coordinates[2] = blue;
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

Color3 Color3::operator+(const Color3& other) const
{
    return Color3(r + other.r, g + other.g, b + other.b);
}

Color3 Color3::operator-(const Color3& other) const
{
    return Color3(r - other.r, g - other.g, b - other.b);
}

void Color3::operator+=(const Color3& other)
{
    r += other.r;
    g += other.g;
    b += other.b;
}

void Color3::operator-=(const Color3& other)
{
    r -= other.r;
    g -= other.g;
    b -= other.b;
}

Color3 Color3::operator/(double scalar) const
{
    double inv = 1 / scalar;
    return Color3(r * inv, g * inv, b * inv);
}

void Color3::operator/=(double scalar)
{
    r /= scalar;
    g /= scalar;
    b /= scalar;
}

void Color3::operator*=(double scalar)
{
    r *= scalar;
    g *= scalar;
    b *= scalar;
}

Color3 operator*(const Color3& a, const Color3& b)
{
    return Color3( a.r * b.r, a.g * b.g, a.b * b.b );
}
Color3 operator*(const Color3& c, double scalar)
{
    return Color3(c.r * scalar, c.g * scalar, c.b * scalar);
}
Color3 operator*(double scalar, const Color3& c)
{
    return c * scalar;
}
