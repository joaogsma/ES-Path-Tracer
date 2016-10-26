#ifndef __GUARD_RECOMBINATION_H
#define __GUARD_RECOMBINATION_H

#include "individual.h"
#include "random_engine.h"

namespace ES {
	Individual discrete_recombination(const Individual& parent1, const Individual& parent2);

	Individual interpolation_recombination(const Individual& parent1, const Individual& parent2);

	Individual hibrid_recombination(const Individual& parent1, const Individual& parent2);
}

#endif
