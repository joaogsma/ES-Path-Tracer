#ifndef __GUARD_RANDOM_SEQUENCE_H__
#define __GUARD_RANDOM_SEQUENCE_H__

#include "geometry/vector3.h"

class Random_Sequence
{
public:
    virtual double next() = 0;
    virtual Vector3 cos_hemisphere_random(const Vector3& n) = 0;
}

#endif
