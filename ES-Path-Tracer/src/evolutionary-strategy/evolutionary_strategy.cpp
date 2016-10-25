#include <vector>

#include "evolutionary-strategy/evolutionary_strategy.h"
#include "evolutionary-strategy/individual.h"

using std::vector;

namespace ES
{
	Evo_Strategy::Evo_Strategy(int iteration_limit, 
		vector<Individual>::size_type population_size)
		: iteration_limit(iteration_limit), population_size(population_size) {}

	Evo_Strategy::Evo_Strategy( int iteration_limit, int n_indv_obj_attr,
		vector<Individual>::size_type population_size, 
		stop_condition_function stop_condition_fn,
		parent_selection_function parent_selection_fn,
		recombination_function recombination_fn,
		fitness_function fitness_fn,
		survivor_selection_function survivor_selection_fn )
		: iteration_limit(iteration_limit), population_size(population_size), 
		n_indv_obj_attr(n_indv_obj_attr), stop_condition_fn(stop_condition_fn), 
		parent_selection_fn(parent_selection_fn), recombination_fn(recombination_fn), 
		fitness_fn(fitness_fn), survivor_selection_fn(survivor_selection_fn)
	{
		initialize();
	}

	bool Evo_Strategy::initialize()
	{
		// Return false if some parameter function pointer is unspecified
		if ( stop_condition_fn == nullptr || parent_selection_fn == nullptr || 
			recombination_fn == nullptr || fitness_fn == nullptr || 
			survivor_selection_fn == nullptr )
		{
			return false;
		}

		// Initialize the population
		for (vector<Individual>::size_type i = 0; i < population_size; i++)
			population.push_back(Individual(n_indv_obj_attr, ES::mt_engine, fitness_fn));

		initialized = true;

		return true;
	}

	void Evo_Strategy::iterate()
	{
		// TODO...
	}

	vector<Individual> Evo_Strategy::evolve()
	{
		int iteration_num = 0;
		
		while ( !stop_condition_fn(population, iteration_num++, iteration_limit) )
			iterate();
		
		return population;
	}
}