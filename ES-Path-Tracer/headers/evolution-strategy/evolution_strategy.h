#ifndef __GUARD_evolution_STRATEGY_H__
#define __GUARD_evolution_STRATEGY_H__

#include <functional>
#include <utility>
#include <vector>

#include "individual.h"

namespace es
{
	class Evolution_Strategy {
	public:
		// Typedefs
		typedef std::function<bool(const std::vector<Individual>& population,
			int completed_iterations,
			int iteration_limit)>
			stop_condition_function;
		typedef std::function<std::pair<Individual, Individual>(
			const std::vector<Individual>& population)>
			parent_selection_function;
		typedef std::function<Individual(const Individual& parent0, const Individual& parent1)>
			recombination_function;
		typedef std::function<double(const Individual& individual)> fitness_function;
		typedef std::function<void(
			std::vector<Individual>& population,
			std::vector<Individual> children)>
			survivor_selection_function;

		// Public members
		std::vector<Individual>::size_type population_size;
		stop_condition_function stop_condition_fn;
		parent_selection_function parent_selection_fn;
		recombination_function recombination_fn;
		fitness_function fitness_fn;
		survivor_selection_function survivor_selection_fn;

		Evolution_Strategy(int m_iteration_limit, int m_children_population_ratio,
			std::vector<Individual>::size_type population_size);

		Evolution_Strategy(int m_iteration_limit, int m_num_individual_obj_var,
			int m_children_population_ratio,
			std::vector<Individual>::size_type population_size,
			stop_condition_function stop_condition_fn,
			parent_selection_function parent_selection_fn,
			recombination_function recombination_fn,
			fitness_function fitness_fn,
			survivor_selection_function survivor_selection_fn);

		// Initializes the m_population
		bool initialize();

		std::vector<Individual> evolve();

	private:
		std::vector<Individual> m_population;
		int m_iteration_limit;
		int m_num_individual_obj_var;
		double m_children_population_ratio;
		bool m_initialized;
		
		// Runs an single iteration
		void iterate();
	};
}

#endif