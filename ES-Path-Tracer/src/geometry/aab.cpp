#include <cmath>
#include "geometry/aab.h"

static const double epsilon = 10e-6;

bool operator==(const AAB &a, const AAB &b)
{
    return std::abs(a.min_x - b.min_x) < epsilon && std::abs(a.max_x - b.max_x) < epsilon &&
        std::abs(a.min_y - b.min_y) < epsilon && std::abs(a.max_y - b.max_y) < epsilon &&
        std::abs(a.min_z - b.min_z) < epsilon && std::abs(a.max_z - b.max_z) < epsilon;
}