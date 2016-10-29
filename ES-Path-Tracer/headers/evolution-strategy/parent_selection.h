#ifndef __GUARD_PARENT_SELECTION_H__
#define __GUARD_PARENT_SELECTION_H__

#include <random>
#include <utility>
#include <vector>

#include "individual.h"
#include "random_engine.h"

namespace es
{
    namespace parent_selection
    {
        std::pair<individual::Individual, individual::Individual> local_uniform_selection(
            const std::vector<individual::Individual>& population);

        std::pair<individual::Individual, individual::Individual> global_uniform_selection(
            const std::vector<individual::Individual>& population);
    }
}

#endif
