#ifndef ES_PATH_TRACER__EVOLUTION_STRATEGY__STOP_CONDITION_H_
#define ES_PATH_TRACER__EVOLUTION_STRATEGY__STOP_CONDITION_H_

#include <vector>

#include "individual.h"

namespace es
{
	class Stop_Condition {
	public:
		static bool max_iterations_stop(
			const std::vector<Individual>& population,
			int completed_iterations,
			int iteration_limit);

	private:
		Stop_Condition();
	};
}

#endif
