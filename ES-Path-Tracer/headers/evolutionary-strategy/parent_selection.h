#ifndef __GUARD_PARENT_SELECTION_H__
#define __GUARD_PARENT_SELECTION_H__

#include <random>
#include <utility>
#include <vector>

#include "individual.h"

namespace ES
{
	// ============================================================================
	// ============================ UTILITY FUNCTIONS =============================
	// ============================================================================

	// ============================================================================
	// ======================= PARENT SELECTION FUNCTIONS =========================
	// ============================================================================

	std::pair<Individual, Individual> local_uniform_selection(
		const std::vector<Individual>& population, const std::mt19937& random_engine);

	std::pair<Individual, Individual> global_uniform_selection(
		const std::vector<Individual>& population, const std::mt19937& random_engine);
}

#endif
