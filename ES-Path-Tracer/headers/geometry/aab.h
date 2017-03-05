#ifndef __GUARD_AAB_H__
#define __GUARD_AAB_H__

#include "point3.h"

class AAB {
public:
    double min_x, max_x, min_y, max_y, min_z, max_z;

    AAB() : min_x(0), max_x(0), min_y(0), max_y(0), min_z(0), max_z(0) {}

    AAB(double min_x, double max_x, double min_y, double max_y, double min_z, double max_z)
        : min_x(min_x), max_x(max_x), min_y(min_y), max_y(max_y), min_z(min_z), max_z(max_z) {}
};

bool operator==(const AAB &a, const AAB &b);

#endif
