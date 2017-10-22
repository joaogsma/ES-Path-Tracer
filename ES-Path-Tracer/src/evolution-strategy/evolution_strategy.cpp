#include <stdexcept>
#include <vector>
#include <utility>

#include "evolution-strategy/evolution_strategy.h"
#include "evolution-strategy/individual.h"

using std::logic_error;
using std::vector;
using std::pair;
using es::Individual;

namespace es
{
	Evolution_Strategy::Evolution_Strategy(int m_iteration_limit, int m_children_population_ratio,
		vector<Individual>::size_type population_size)
		: m_iteration_limit(m_iteration_limit), population_size(population_size) {}

	Evolution_Strategy::Evolution_Strategy( int m_iteration_limit, int m_num_individual_obj_var,
		int m_children_population_ratio,
		vector<Individual>::size_type population_size, 
		stop_condition_function stop_condition_fn,
		parent_selection_function parent_selection_fn,
		recombination_function recombination_fn,
		fitness_function fitness_fn,
		survivor_selection_function survivor_selection_fn )
		: m_iteration_limit(m_iteration_limit), population_size(population_size), 
		m_children_population_ratio(m_children_population_ratio),
		m_num_individual_obj_var(m_num_individual_obj_var), stop_condition_fn(stop_condition_fn), 
		parent_selection_fn(parent_selection_fn), recombination_fn(recombination_fn), 
		fitness_fn(fitness_fn), survivor_selection_fn(survivor_selection_fn)
	{
		initialize();
	}

	bool Evolution_Strategy::initialize()
	{
		// Return false if some parameter function pointer is unspecified
		if (stop_condition_fn == nullptr || parent_selection_fn == nullptr || 
			recombination_fn == nullptr || fitness_fn == nullptr || 
			survivor_selection_fn == nullptr)
		{
			return false;
		}

		// Initialize the m_population
		for (vector<Individual>::size_type i = 0; i < population_size; i++)
			m_population.push_back(Individual(m_num_individual_obj_var, fitness_fn));

		m_initialized = true;

		return true;
	}

	void Evolution_Strategy::iterate()
	{
		vector<Individual> children;
		double children_size = m_children_population_ratio * population_size;
		children.reserve( (vector<Individual>::size_type) children_size );

		while (children.size() < m_children_population_ratio * population_size)
		{
			// Select parents
			pair<Individual, Individual>& parents = parent_selection_fn(m_population);
			// Generate a child
			Individual& child = recombination_fn(parents.first, parents.second);
			// Mutate the child
			child.mutate();
			children.push_back(child);
		}

		// Select populaiton for the next iteration
		survivor_selection_fn(m_population, children);
	}

	vector<Individual> Evolution_Strategy::evolve()
	{
		if ( !m_initialized )
			throw logic_error("evolution Strategy not m_initialized");

		int iteration_num = 0;
		
		while ( !stop_condition_fn(m_population, iteration_num++, m_iteration_limit) )
			iterate();
		
		return m_population;
	}
}