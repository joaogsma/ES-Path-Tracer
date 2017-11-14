#ifndef ES_PATH_TRACER__RANDOM__ES_INDIVIDUAL_RANDOM_SEQUENCE_H_
#define ES_PATH_TRACER__RANDOM__ES_INDIVIDUAL_RANDOM_SEQUENCE_H_

#include "../evolution-strategy/individual.h"
#include "../geometry/vector3.h"
#include "random_sequence.h"

namespace random
{
	class ES_Individual_Random_Sequence : public Random_Sequence {
	public:
		ES_Individual_Random_Sequence(es::Individual& individual);

		double next();

		virtual Vector3 uniform_distributed_hemisphere_sample();

		virtual Vector3 cos_distributed_hemisphere_sample();
	
	private:
		enum Operation { NONE, NEXT, HEMISPHERE_SAMPLE };
		
		es::Individual& m_individual;
		es::Individual::size_type m_position;
		Operation m_previous_operation;

		double next_element();

		void ensure_block_length();
	};
}

#endif
