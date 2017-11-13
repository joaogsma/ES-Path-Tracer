#include <algorithm>
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
	Evolution_Strategy::Evolution_Strategy(
		int iteration_limit,
		int num_individual_obj_var,
		double children_population_ratio,
		vector<Individual>::size_type m_population_size, 
		stop_condition_function stop_condition_fn,
		parent_selection_function parent_selection_fn,
		recombination_function recombination_fn,
		fitness_function* fitness_fn,
		survivor_selection_function survivor_selection_fn)
		: m_iteration_limit(iteration_limit),
		m_population_size(m_population_size), 
		m_children_population_ratio(children_population_ratio),
		m_num_individual_obj_var(num_individual_obj_var),
		m_stop_condition_fn(stop_condition_fn), 
		m_parent_selection_fn(parent_selection_fn),
		m_recombination_fn(recombination_fn), 
		m_fitness_fn(fitness_fn),
		m_survivor_selection_fn(survivor_selection_fn),
		m_has_evolved(false)
	{
		if (iteration_limit <= 0)
			throw std::invalid_argument("Invalid maximum iteration limit");

		if (num_individual_obj_var <= 0)
			throw std::invalid_argument("Invalid number of object variables per individual");

		if (children_population_ratio <= 0)
			throw std::invalid_argument("Invalid children to population size ratio");

		if (m_population_size < 2)
			throw std::invalid_argument("Invalid population size");

		if (!stop_condition_fn)
			throw std::invalid_argument("Invalid stop condition function");

		if (!parent_selection_fn)
			throw std::invalid_argument("Invalid parent selection function");

		if (!recombination_fn)
			throw std::invalid_argument("Invalid recombination function");

		if (!fitness_fn)
			throw std::invalid_argument("Invalid fitness function");

		if (!survivor_selection_fn)
			throw std::invalid_argument("Invalid survivor selection function");
	}

	void Evolution_Strategy::initialize()
	{
		while (m_population.size() < m_population_size)
		{
			Individual new_individual((Individual::size_type) m_num_individual_obj_var, m_fitness_fn);
			m_population.push_back(new_individual);
		}
	}

	void Evolution_Strategy::iterate()
	{
		vector<Individual> children;
		vector<Individual>::size_type n_children =
			(vector<Individual>::size_type) m_children_population_ratio * m_population_size;
		children.reserve(n_children);

		while (children.size() < n_children)
		{
			// Select parents
			pair<Individual, Individual>& parents = m_parent_selection_fn(m_population);
			// Generate a child
			Individual& child = m_recombination_fn(parents.first, parents.second);
			// Mutate the child
			child.mutate();
			children.push_back(child);
		}

		// Select populaiton for the next iteration
		m_survivor_selection_fn(m_population, children);

		// Ensure all members of the population have the same size
		const Individual& longest_individual =
			*std::max_element(m_population.begin(), m_population.end(), smaller_size_predicate);
		Individual::size_type largest_size = longest_individual.size();
		for (Individual& individual : m_population)
		{
			Individual::size_type remaining = largest_size - individual.size();
			if (remaining > 0)
				individual.expand(remaining);
		}
	}

	void Evolution_Strategy::evolve()
	{
		if (m_has_evolved)
			return;
		
		initialize();

		int iteration_num = 0;
		
		while (!m_stop_condition_fn(m_population, iteration_num++, m_iteration_limit))
			iterate();

		m_has_evolved = true;
	}

	bool Evolution_Strategy::smaller_size_predicate(
		const Individual& individual0,
		const Individual& individual1)
	{
		return individual0.size() < individual1.size();
	}

	const Evolution_Strategy::stop_condition_function& Evolution_Strategy::stop_condition_fn() const
	{
		return m_stop_condition_fn;
	}

	const Evolution_Strategy::parent_selection_function& Evolution_Strategy::parent_selection_fn() const
	{
		return m_parent_selection_fn;
	}

	const Evolution_Strategy::recombination_function& Evolution_Strategy::recombination_fn() const
	{
		return m_recombination_fn;
	}

	const Evolution_Strategy::fitness_function* Evolution_Strategy::fitness_fn() const
	{
		return m_fitness_fn;
	}

	const Evolution_Strategy::survivor_selection_function& Evolution_Strategy::survivor_selection_fn() const
	{
		return m_survivor_selection_fn;
	}

	const std::vector<Individual>& Evolution_Strategy::population() const
	{
		return m_population;
	}

	const std::vector<Individual>::size_type& Evolution_Strategy::population_size() const
	{
		return m_population_size;
	}

	const int Evolution_Strategy::iteration_limit() const
	{
		return m_iteration_limit;
	}

	const int Evolution_Strategy::num_individual_obj_var() const
	{
		return m_num_individual_obj_var;
	}

	const double Evolution_Strategy::children_population_ratio() const
	{
		return m_children_population_ratio;
	}

	const bool Evolution_Strategy::has_evolved() const
	{
		return m_has_evolved;
	}
}