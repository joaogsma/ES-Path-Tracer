#include <stdexcept>

#include "evolution-strategy/individual.h"
#include "geometry/vector3.h"
#include "random/es_individual_random_sequence.h"

namespace random
{
	ES_Individual_Random_Sequence::ES_Individual_Random_Sequence(es::Individual& individual)
		: m_individual(individual), m_position(0), m_previous_operation(Operation::NONE)
	{
		if (individual.size() == 0)
			throw std::invalid_argument("Empty individual");
	}

	double ES_Individual_Random_Sequence::next()
	{
		if (m_previous_operation == Operation::NEXT)
		{
			// m_position is in an incorrect position for a next() operation - advance it by 2
			m_position += 2;
		}
		
		ensure_block_length();
		
		m_previous_operation = Operation::NEXT;
		return m_individual[m_position++];
	}

	double ES_Individual_Random_Sequence::next_element()
	{
		if (m_previous_operation == HEMISPHERE_SAMPLE)
		{
			// m_position is in an incorrect position for either a uniform_distributed_hemisphere_sample
			// or a cos_distributed_hemisphere_sample operation - advance it by 1
			++m_position;
		}

		ensure_block_length();

		return m_individual[m_position++];
	}

	void ES_Individual_Random_Sequence::ensure_block_length()
	{
		es::Individual::size_type remaining_positions = m_individual.size() - m_position;
		es::Individual::size_type expected_positions = 4 - (m_position % 3);
		// Adjust size
		if (remaining_positions < expected_positions)
		{
			int expansion_size = int(expected_positions - remaining_positions);
			m_individual.expand(expansion_size);
		}
	}

	Vector3 ES_Individual_Random_Sequence::uniform_distributed_hemisphere_sample()
	{
		Vector3 value = Random_Sequence::uniform_distributed_hemisphere_sample();
		m_previous_operation = Operation::HEMISPHERE_SAMPLE;
		return value;
	}

	Vector3 ES_Individual_Random_Sequence::cos_distributed_hemisphere_sample()
	{
		Vector3 value = Random_Sequence::cos_distributed_hemisphere_sample();
		m_previous_operation = Operation::HEMISPHERE_SAMPLE;
		return value;
	}
}