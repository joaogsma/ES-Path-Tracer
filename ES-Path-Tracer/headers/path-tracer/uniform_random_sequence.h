#ifndef __GUARD_UNIFORM_RANDOM_SEQUENCE_H__
#define __GUARD_UNIFORM_RANDOM_SEQUENCE_H__

#include <random>
#include <vector>

#include "geometry/vector3.h"
#include "random_sequence.h"
#include "scene/surface_element.h"

class Uniform_Random_Sequence : public Random_Sequence {
public:
    Uniform_Random_Sequence() {}
    Uniform_Random_Sequence(const std::vector<double>& values) : sequence(values) {}
    
    double next();
    Vector3 cos_hemisphere_random(const scene::Surface_Element::Shading_Data& shading_data);

private:
    std::vector<double> sequence;
};

#endif
