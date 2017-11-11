#ifndef __GUARD_STOP_CONDITION_H__
#define __GUARD_STOP_CONDITION_H__

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
