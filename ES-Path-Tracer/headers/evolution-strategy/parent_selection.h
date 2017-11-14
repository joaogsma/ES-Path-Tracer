#ifndef ES_PATH_TRACER__EVOLUTION_STRATEGY__PARENT_SELECTION_H_
#define ES_PATH_TRACER__EVOLUTION_STRATEGY__PARENT_SELECTION_H_

#include <functional>
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
		Parent_Selection();

		static void global_fill_vector(
			std::vector<double>& parent1_vec,
			std::vector<double>& parent2_vec,
			const std::vector<Individual>& population,
			std::function<Individual::const_iterator(const Individual&)> begin_it,
			std::function<Individual::const_iterator(const Individual&)> end_it);

		static Individual::const_iterator object_var_begin(const Individual& individual);
		static Individual::const_iterator object_var_end(const Individual& individual);
		static Individual::const_iterator step_size_begin(const Individual& individual);
		static Individual::const_iterator step_size_end(const Individual& individual);
	};
}

#endif
