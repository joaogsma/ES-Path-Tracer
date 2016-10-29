#ifndef __GUARD_evolution_STRATEGY_H__
#define __GUARD_evolution_STRATEGY_H__

#include <utility>
#include <vector>

#include "fitness.h"
#include "individual.h"
#include "parent_selection.h"
#include "recombination.h"
#include "survivor_selection.h"

namespace es
{
	// ===========================================================================
	// ================================ TYPEDEFS =================================
	// ===========================================================================
	
	typedef bool (*stop_condition_function)(
        const std::vector<individual::Individual>& population,
		int completed_iterations, int iteration_limit);

	typedef std::pair<individual::Individual, individual::Individual> 
        (*parent_selection_function)(
		const std::vector<individual::Individual>& population);

	typedef individual::Individual (*recombination_function)(
        const individual::Individual& parent1, 
        const individual::Individual& parent2);

	typedef double (*fitness_function)(const individual::Individual& individual);

	typedef void (*survivor_selection_function)(
        std::vector<individual::Individual>& population,
		std::vector<individual::Individual> children);
	
	// ===========================================================================



	// ===========================================================================
	// ========================== evolution STRATEGY ==========================
	// ===========================================================================
	
	class Evo_Strategy {
	public:
		std::vector<individual::Individual>::size_type population_size;
		
		/*	Evolution function parameters, which are pointers to functions that 
			implement the different steps in the ES */
		stop_condition_function stop_condition_fn;
		parent_selection_function parent_selection_fn;
		recombination_function recombination_fn;
		fitness_function fitness_fn;
		survivor_selection_function survivor_selection_fn;

		Evo_Strategy(int iteration_limit, int children_population_ratio,
			std::vector<individual::Individual>::size_type population_size);

		Evo_Strategy(int iteration_limit, int n_indv_obj_var,
			int children_population_ratio,
			std::vector<individual::Individual>::size_type population_size,
			stop_condition_function stop_condition_fn,
			parent_selection_function parent_selection_fn,
			recombination_function recombination_fn,
			fitness_function fitness_fn,
			survivor_selection_function survivor_selection_fn);

		// Initializes the population
		bool initialize();

		std::vector<individual::Individual> evolve();

	private:
		std::vector<individual::Individual> population;
		int iteration_limit;
		int n_indv_obj_var;
		double children_population_ratio;
		bool initialized;
		
		// Runs an single iteration
		void iterate();
	};

	// ===========================================================================
}

#endif