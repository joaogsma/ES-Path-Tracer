#define _USE_MATH_DEFINES

#include <cmath>
#include <random>
#include <vector>

#include "geometry/vector3.h"
#include "random/uniform_random_sequence.h"
#include "shading/surface_element.h"

namespace random
{
	double Uniform_Random_Sequence::next()
	{
		if (m_index < sequence.size())
			return sequence[m_index++];
    
		std::uniform_real_distribution<double> dist(0.0, 1.0);
		double r = dist(m_mt_engine);

		sequence.push_back(r);
		++m_index;

		return r;
	}

	Vector3 Uniform_Random_Sequence::uniform_distributed_hemisphere_sample()
	{
		std::uniform_real_distribution<double> dist(0.0, 1.0);

		while (sequence.size() - m_index < 2)
			sequence.push_back(dist(m_mt_engine));
	
		double u = sequence[m_index++];
		double v = sequence[m_index++];
		double r = sqrt(1 - v*v);

		return Vector3(r * cos(2 * M_PI * u), v, r * sin(2 * M_PI * u));
	}

	Vector3 Uniform_Random_Sequence::cos_distributed_hemisphere_sample()
	{
		std::uniform_real_distribution<double> dist(0.0, 1.0);

		while(sequence.size() - m_index < 2)
			sequence.push_back(dist(m_mt_engine));

		double theta = sequence[m_index++] * 2 * M_PI;
		double s = sequence[m_index++];
    
		double y = sqrt(s);
		double r = sqrt(1.0 - y*y);

		return Vector3(r * cos(theta), y, r * sin(theta));
	}
}