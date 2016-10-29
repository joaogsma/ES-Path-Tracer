#ifndef __GUARD_SURVIVOR_SELECTION_H
#define __GUARD_SURVIVOR_SELECTION_H

#include <vector>

#include "individual.h"

namespace es
{
    namespace survivor_selection
    {
        void generational_selection(
            std::vector<individual::Individual>& population, 
            std::vector<individual::Individual> children);

        void nongenerational_selection(
            std::vector<individual::Individual>& population,
            std::vector<individual::Individual> children);
    }
}

#endif