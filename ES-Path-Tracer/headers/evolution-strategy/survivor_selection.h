#ifndef __GUARD_SURVIVOR_SELECTION_H
#define __GUARD_SURVIVOR_SELECTION_H

#include <vector>

#include "individual.h"

namespace es
{
    class Survivor_Selection {
	public:
		static void generational_selection(
            std::vector<Individual>& m_population, 
            std::vector<Individual> children);

        static void nongenerational_selection(
            std::vector<Individual>& m_population,
            std::vector<Individual> children);

	private:
		Survivor_Selection();

		static bool compare(Individual& indiv1, Individual& indiv2);
		
		static void Survivor_Selection::select_survivors(
			std::vector<Individual>& individual_vec,
			std::vector<Individual>::size_type num_survivors);
	};
}

#endif