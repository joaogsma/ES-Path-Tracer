#ifndef ES_PATH_TRACER__RANDOM__RANDOM_SEQUENCE_H_
#define ES_PATH_TRACER__RANDOM__RANDOM_SEQUENCE_H_

#include <random>

#include "../geometry/vector3.h"
#include "random_number_engine.h"

namespace random
{
	class Random_Sequence {
	public:
		Random_Sequence() : m_mt_engine(random::mt_engine_singleton()), m_index(0) {}

		virtual double next() = 0;
		
		virtual Vector3 uniform_distributed_hemisphere_sample();
		
		virtual Vector3 cos_distributed_hemisphere_sample();

	protected:
		std::mt19937& m_mt_engine;
		size_t m_index;

		virtual double next_element();
	};
}

#endif
