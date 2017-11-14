#ifndef ES_PATH_TRACER__RANDOM__UNIFORM_RANDOM_SEQUENCE_H_
#define ES_PATH_TRACER__RANDOM__UNIFORM_RANDOM_SEQUENCE_H_

#include <random>
#include <vector>

#include "random_sequence.h"

namespace random
{
	class Uniform_Random_Sequence : public Random_Sequence {
	public:
		Uniform_Random_Sequence() {}
		Uniform_Random_Sequence(const std::vector<double>& values) : sequence(values) {}
    
		double next();
		
	private:
		std::vector<double> sequence;
	};
}

#endif
