#ifndef __GUARD_RANDOM_SEQUENCE_H__
#define __GUARD_RANDOM_SEQUENCE_H__

#include <random>

#include "common/random_number_engine.h"
#include "geometry/vector3.h"

class Random_Sequence
{
public:
    Random_Sequence() : m_mt_engine(random::mt_engine_singleton()), m_index(0) {}

    virtual double next() = 0;
	virtual Vector3 uniform_distributed_hemisphere_sample() = 0;
    virtual Vector3 cos_distributed_hemisphere_sample() = 0;

protected:
    std::mt19937& m_mt_engine;
    size_t m_index;
};

#endif
