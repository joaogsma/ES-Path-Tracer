#define _USE_MATH_DEFINES

#include <cmath>
#include <random>
#include <vector>

#include "geometry/vector3.h"
#include "path-tracer/uniform_random_sequence.h"
#include "scene/surface_element.h"

double Uniform_Random_Sequence::next()
{
    if ( index < sequence.size() )
        return sequence[index++];
    
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double r = dist(mt_engine);

    sequence.push_back(r);
    ++index;

    return r;
}

Vector3 Uniform_Random_Sequence::cos_hemisphere_random(
    const scene::Surface_Element::Shading_Data& shading_data)
{
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    double theta, s;

    if ( index < sequence.size() )
        theta = sequence[index++];
    else
    {
        theta = dist(mt_engine);
        sequence.push_back(theta);
        ++index;
    }
    
    if ( index < sequence.size() )
        s = sequence[index++];
    else
    {
        s = dist(mt_engine);
        sequence.push_back(s);
        ++index;
    }

    theta *= 2 * M_PI;

    double y = sqrt(s);
    double r = sqrt(1.0 - y*y);

    Vector3 sample( r * cos(theta), y, r * sin(theta) );

    return (sample[0] * shading_data.tangent0) + (sample[1] * shading_data.normal) + 
        (sample[2] * shading_data.tangent1);
}