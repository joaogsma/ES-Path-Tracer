#include <cmath>
#include <random>
#include <vector>

#include "random/uniform_random_sequence.h"

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
}