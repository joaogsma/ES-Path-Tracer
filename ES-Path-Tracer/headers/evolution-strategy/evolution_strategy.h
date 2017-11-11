#ifndef __GUARD_evolution_STRATEGY_H__
#define __GUARD_evolution_STRATEGY_H__

#include <functional>
#include <utility>
#include <vector>

namespace es
{
	class Individual;    // Forward declaration

	class Evolution_Strategy {
	public:
		typedef std::function<bool(const std::vector<Individual>& population,
			int completed_iterations,
			int iteration_limit)>
			stop_condition_function;

		typedef std::function<std::pair<Individual, Individual>(
			const std::vector<Individual>& population)>
			parent_selection_function;
		
		typedef std::function<Individual(const Individual& parent0, const Individual& parent1)>
			recombination_function;
		
		typedef std::function<double(Individual& individual)> fitness_function;
		
		typedef std::function<void(
			std::vector<Individual>& population,
			std::vector<Individual> children)>
			survivor_selection_function;

		Evolution_Strategy(
			int iteration_limit,
			int num_individual_obj_var,
			double children_population_ratio,
			std::vector<Individual>::size_type m_population_size,
			stop_condition_function stop_condition_fn,
			parent_selection_function parent_selection_fn,
			recombination_function recombination_fn,
			fitness_function* fitness_fn,
			survivor_selection_function survivor_selection_fn);

		void evolve();

		// Accessor functions
		const stop_condition_function& stop_condition_fn() const;
		const parent_selection_function& parent_selection_fn() const;
		const recombination_function& recombination_fn() const;
		const fitness_function* fitness_fn() const;
		const survivor_selection_function& survivor_selection_fn() const;
		const std::vector<Individual>& population() const;
		const std::vector<Individual>::size_type& population_size() const;
		const int iteration_limit() const;
		const int num_individual_obj_var() const;
		const double children_population_ratio() const;
		const bool has_evolved() const;

	private:
		stop_condition_function m_stop_condition_fn;
		parent_selection_function m_parent_selection_fn;
		recombination_function m_recombination_fn;
		fitness_function* m_fitness_fn;
		survivor_selection_function m_survivor_selection_fn;
		std::vector<Individual> m_population;
		std::vector<Individual>::size_type m_population_size;
		int m_iteration_limit;
		int m_num_individual_obj_var;
		double m_children_population_ratio;
		bool m_has_evolved;
		
		void initialize();
		
		void iterate();

		static bool smaller_size_predicate(
			const Individual& individual0,
			const Individual& individual1);
	};
}

#include "individual.h"

#endif