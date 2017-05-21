#ifndef __GUARD_RANDOM_SEQUENCE_H__
#define __GUARD_RANDOM_SEQUENCE_H__

#include <random>

#include "geometry/vector3.h"
#include "scene/surface_element.h"

class Random_Sequence
{
public:
    Random_Sequence() : mt_engine(std::random_device()()), index(0) {}

    virtual double next() = 0;
    virtual Vector3 cos_hemisphere_random(const scene::Surface_Element::Shading_Data& shading_data) = 0;

protected:
    std::mt19937 mt_engine;
    size_t index;
}

#endif
