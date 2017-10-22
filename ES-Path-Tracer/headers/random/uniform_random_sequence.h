#ifndef __GUARD_UNIFORM_RANDOM_SEQUENCE_H__
#define __GUARD_UNIFORM_RANDOM_SEQUENCE_H__

#include <random>
#include <vector>

#include "../geometry/vector3.h"
#include "../shading/surface_element.h"
#include "random_sequence.h"

namespace random
{
	class Uniform_Random_Sequence : public Random_Sequence {
	public:
		Uniform_Random_Sequence() {}
		Uniform_Random_Sequence(const std::vector<double>& values) : sequence(values) {}
    
		double next();
		Vector3 uniform_distributed_hemisphere_sample();
		Vector3 cos_distributed_hemisphere_sample();

	private:
		std::vector<double> sequence;
	};
}

#endif
