#include <stdexcept>
#include <vector>
#include <utility>

#include "evolution-strategy/evolution_strategy.h"
#include "evolution-strategy/individual.h"

using std::logic_error;
using std::vector;
using std::pair;

using es::individual::Individual;

namespace es
{
	Evo_Strategy::Evo_Strategy(int iteration_limit, int children_population_ratio,
		vector<Individual>::size_type population_size)
		: iteration_limit(iteration_limit), population_size(population_size) {}

	Evo_Strategy::Evo_Strategy( int iteration_limit, int n_indv_obj_var,
		int children_population_ratio,
		vector<Individual>::size_type population_size, 
		stop_condition_function stop_condition_fn,
		parent_selection_function parent_selection_fn,
		recombination_function recombination_fn,
		fitness_function fitness_fn,
		survivor_selection_function survivor_selection_fn )
		: iteration_limit(iteration_limit), population_size(population_size), 
		children_population_ratio(children_population_ratio),
		n_indv_obj_var(n_indv_obj_var), stop_condition_fn(stop_condition_fn), 
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
			population.push_back(Individual(n_indv_obj_var, fitness_fn));

		initialized = true;

		return true;
	}

	void Evo_Strategy::iterate()
	{
		vector<Individual> children;
		double children_size = children_population_ratio * population_size;
		children.reserve( (vector<Individual>::size_type) children_size );

		while (children.size() < children_population_ratio * population_size)
		{
			// Select parents
			pair<Individual, Individual>& parents = parent_selection_fn(population);
			// Generate a child
			Individual& child = recombination_fn(parents.first, parents.second);
			// Mutate the child
			child.mutate();
			children.push_back(child);
		}

		// Select populaiton for the next iteration
		survivor_selection_fn(population, children);
	}

	vector<Individual> Evo_Strategy::evolve()
	{
		if ( !initialized )
			throw logic_error("evolution Strategy not initialized");

		int iteration_num = 0;
		
		while ( !stop_condition_fn(population, iteration_num++, iteration_limit) )
			iterate();
		
		return population;
	}
}