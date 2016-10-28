#ifndef __GUARD_PARENT_SELECTION_H__
#define __GUARD_PARENT_SELECTION_H__

#include <random>
#include <utility>
#include <vector>

#include "individual.h"
#include "random_engine.h"

namespace ES
{
    namespace parent_selection
    {
        std::pair<Individual, Individual> local_uniform_selection(
            const std::vector<Individual>& population);

        std::pair<Individual, Individual> global_uniform_selection(
            const std::vector<Individual>& population);
    }
}

#endif
