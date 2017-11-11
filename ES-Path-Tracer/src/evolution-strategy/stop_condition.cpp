#include <stdexcept>
#include <vector>

#include "evolution-strategy/stop_condition.h"
#include "evolution-strategy/individual.h"

namespace es
{
	Stop_Condition::Stop_Condition()
	{
		throw std::runtime_error("Class must not be instantiated");
	}

	bool Stop_Condition::max_iterations_stop(
		const std::vector<Individual>& population,
		int completed_iterations,
		int iteration_limit)
	{
		return completed_iterations >= iteration_limit;
	}
}