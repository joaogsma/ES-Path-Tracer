#define _USE_MATH_DEFINES

#include <cmath>
#include <random>
#include <vector>

#include "geometry/vector3.h"
#include "path-tracer/uniform_random_sequence.h"
#include "shading/surface_element.h"

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

Vector3 Uniform_Random_Sequence::uniform_distributed_hemisphere_sample(
	const scene::Surface_Element::Geometric_Data& geometric_data)
{
	std::uniform_real_distribution<double> dist(0.0, 1.0);

	while (sequence.size() - index < 2)
		sequence.push_back(dist(mt_engine));
	
	double u = sequence[index++];
	double v = sequence[index++];
	double r = sqrt(1 - v*v);

	Vector3 sample(r * cos(2 * M_PI * u), v, r * sin(2 * M_PI * u));

	return (sample[0] * geometric_data.tangent0)
		+ (sample[1] * geometric_data.normal)
		+ (sample[2] * geometric_data.tangent1);
}

Vector3 Uniform_Random_Sequence::cos_distributed_hemisphere_sample(
    const scene::Surface_Element::Geometric_Data& geometric_data)
{
    std::uniform_real_distribution<double> dist(0.0, 1.0);

	while(sequence.size() - index < 2)
		sequence.push_back(dist(mt_engine));

    double theta, s;

    double theta = sequence[index++] * 2 * M_PI;
    double s = sequence[index++];
    
    double y = sqrt(s);
    double r = sqrt(1.0 - y*y);

    Vector3 sample(r * cos(theta), y, r * sin(theta));

    return (sample[0] * geometric_data.tangent0)
		+ (sample[1] * geometric_data.normal)
		+ (sample[2] * geometric_data.tangent1);
}