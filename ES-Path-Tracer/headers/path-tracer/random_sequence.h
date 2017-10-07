#ifndef __GUARD_RANDOM_SEQUENCE_H__
#define __GUARD_RANDOM_SEQUENCE_H__

#include <random>

#include "geometry/vector3.h"

class Shading_Data;

class Random_Sequence
{
public:
    Random_Sequence() : mt_engine(std::random_device()()), index(0) {}

    virtual double next() = 0;
	virtual Vector3 uniform_distributed_hemisphere_sample(
		const scene::Surface_Element::Geometric_Data& geometric_data) = 0;
    virtual Vector3 cos_distributed_hemisphere_sample(
		const scene::Surface_Element::Geometric_Data& shading_data) = 0;

protected:
    std::mt19937 mt_engine;
    size_t index;
}

// Ate the end to prevent cyclic dependency
#include "shading/surface_element.h"

#endif
