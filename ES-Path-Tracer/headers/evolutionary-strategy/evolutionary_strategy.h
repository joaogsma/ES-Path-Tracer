#ifndef __GUARD_EVOLUTIONARY_STRATEGY_H__
#define __GUARD_EVOLUTIONARY_STRATEGY_H__

#include <random>
#include <utility>
#include <vector>

#include "individual.h"
#include "parent_selection.h"

namespace ES
{
	static std::random_device rd;
	static std::mt19937 mt_engine( rd() );

	// ===========================================================================
	// ================================ TYPEDEFS =================================
	// ===========================================================================
	
	typedef bool(*stop_condition_function)(const std::vector<Individual>& population,
		int completed_iterations, int iteration_limit);

	typedef std::pair<Individual, Individual>(*parent_selection_function)(
		const std::vector<Individual>& population, std::mt19937& random_engine);

	typedef Individual(*recombination_function)(
		const std::pair<Individual, Individual>& parents);

	typedef double(*fitness_function)(const Individual& individual);

	typedef void(*survivor_selection_function)(std::vector<Individual>& population,
		const std::vector<Individual>& children);
	
	// ===========================================================================



	// ===========================================================================
	// ========================== EVOLUTIONARY STRATEGY ==========================
	// ===========================================================================
	
	class Evo_Strategy {
	public:
		std::vector<Individual>::size_type population_size;
		
		/*	Evolutionary function parameters, which are pointers to functions that 
			implement the different steps in the ES */
		stop_condition_function stop_condition_fn;
		parent_selection_function parent_selection_fn;
		recombination_function recombination_fn;
		fitness_function fitness_fn;
		survivor_selection_function survivor_selection_fn;

		Evo_Strategy(int iteration_limit, int children_population_ratio,
			std::vector<Individual>::size_type population_size);

		Evo_Strategy(int iteration_limit, int n_indv_obj_attr,
			int children_population_ratio,
			std::vector<Individual>::size_type population_size,
			stop_condition_function stop_condition_fn,
			parent_selection_function parent_selection_fn,
			recombination_function recombination_fn,
			fitness_function fitness_fn,
			survivor_selection_function survivor_selection_fn);

		// Initializes the population
		bool initialize();

		std::vector<Individual> evolve();

	private:
		std::vector<Individual> population;
		int iteration_limit;
		int n_indv_obj_attr;
		double children_population_ratio;
		bool initialized;
		
		// Runs an single iteration
		void iterate();
	};

	// ===========================================================================
}

#endif