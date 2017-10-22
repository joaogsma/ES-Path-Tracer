#ifndef __GUARD_PARENT_SELECTION_H__
#define __GUARD_PARENT_SELECTION_H__

#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "individual.h"

namespace es
{
	class Parent_Selection {
	public:
		static std::pair<Individual, Individual> local_uniform_selection(
			const std::vector<Individual>& m_population);

		static std::pair<Individual, Individual> global_uniform_selection(
			const std::vector<Individual>& m_population);

	private:
		typedef Individual::const_iterator(Individual::*const_iterator_function)() const;

		Parent_Selection();

		static void global_fill_vector(
			std::vector<double>& parent1_vec,
			std::vector<double>& parent2_vec,
			const std::vector<Individual>& population,
			const_iterator_function begin,
			const_iterator_function end);
	};
}

#endif
