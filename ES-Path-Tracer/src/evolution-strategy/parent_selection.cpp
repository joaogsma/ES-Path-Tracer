#include <algorithm>
#include <functional>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "evolution-strategy/individual.h"
#include "evolution-strategy/parent_selection.h"
#include "random/random_number_engine.h"

using std::copy;
using std::function;
using std::logic_error;
using std::make_pair;
using std::mt19937;
using std::pair;
using std::uniform_int_distribution;
using std::vector;
using std::mt19937;

using es::Individual;

namespace es
{
	Parent_Selection::Parent_Selection()
	{
		throw std::runtime_error("Class must not be instantiated");
	}

	Individual::const_iterator Parent_Selection::object_var_begin(const Individual& individual)
	{
		return individual.obj_var_begin();
	}

	Individual::const_iterator Parent_Selection::object_var_end(const Individual& individual)
	{
		return individual.obj_var_end();
	}

	Individual::const_iterator Parent_Selection::step_size_begin(const Individual& individual)
	{
		return individual.step_size_begin();
	}

	Individual::const_iterator Parent_Selection::step_size_end(const Individual& individual)
	{
		return individual.step_size_end();
	}

	// ========================================================================
    // =============================== UTILITY ================================
    // ========================================================================

    /*	This function fills the specified vectors with values globally uniformly
        selected (using the specified next engine) from the given m_population.
        The values from the m_population individuals, as well as the vector sizes,
        are extracted from the intervals specified by the iterator functions */
    void Parent_Selection::global_fill_vector(
		vector<double>& parent1_vec,
		vector<double>& parent2_vec,
        const vector<Individual>& population,
		function<Individual::const_iterator(const Individual&)> begin_it,
        function<Individual::const_iterator(const Individual&)> end_it)
    {
        parent1_vec.clear();
        parent2_vec.clear();

		vector<double>::size_type size = end_it(population[0]) - begin_it(population[0]);

        // Uniform distribution for the m_population vector
        uniform_int_distribution<vector<Individual>::size_type> indiv_dist(0,
            population.size() - 1);

        for (vector<double>::size_type i = 0; i < size; ++i)
        {
            vector<Individual>::size_type parent1_index, parent2_index;
			mt19937& mt_engine = random::mt_engine_singleton();
			// Select a first parent
            parent1_index = indiv_dist(mt_engine);
            // Select a second parent different from the first
            while ((parent2_index = indiv_dist(mt_engine)) == parent1_index);

            // Get the selected parents
            const Individual& parent1 = population[parent1_index];
            const Individual& parent2 = population[parent2_index];

            // Get the values from the parents
            double value_parent1 = begin_it(parent1)[i];
            double value_parent2 = begin_it(parent2)[i];

            // Store the values
            parent1_vec.push_back(value_parent1);
            parent2_vec.push_back(value_parent2);
        }
    }

    // ========================================================================



    // ========================================================================
    // ====================== PARENT SELECTION FUNCTIONS ======================
    // ========================================================================

    /*	Local uniform selection. This function selects two different parents
        with uniform probability from amongst the m_population and returns them */
    pair<Individual, Individual> Parent_Selection::local_uniform_selection(
        const vector<Individual>& m_population)
    {
        if (m_population.empty())
            throw logic_error("Empty m_population");

        // Uniform distribution for the m_population vector
        uniform_int_distribution<vector<Individual>::size_type> indiv_dist(0,
            m_population.size() - 1);

		mt19937 mt_engine = random::mt_engine_singleton();
        // Select a first parent
        vector<Individual>::size_type parent1_index = indiv_dist(mt_engine);
        // Select a second parent different from the first
        vector<Individual>::size_type parent2_index;
        while ((parent2_index = indiv_dist(mt_engine)) == parent1_index);

        return make_pair(m_population[parent1_index], m_population[parent2_index]);
    }

    /*	Global uniform selection. This function selects each object attribute
        and each step size from a uniformly selected individual from
        the m_population, sampled independently for each value. Corresponding
        attributes in both generated parents are guaranteed to come from
        different individuals in the m_population */
    pair<Individual, Individual> Parent_Selection::global_uniform_selection(
        const vector<Individual>& m_population)
    {
        if (m_population.empty())
            throw logic_error("Empty m_population");

        // Vectors for the generated parents's data
        vector<double> object_attributes1, object_attributes2, step_sizes1,
            step_sizes2;

        // Fill the object attributes vectors
		global_fill_vector(
			object_attributes1,
			object_attributes2,
			m_population,
			object_var_begin,
			object_var_end);

        // Fill the step size vectors
		global_fill_vector(step_sizes1, step_sizes2, m_population, step_size_begin, step_size_end);

        int num_obj_var = (int) object_attributes1.size();

        // Create return parents
        pair<Individual, Individual> parents = make_pair(
            Individual(num_obj_var, m_population[0].fitness_function()),
            Individual(num_obj_var, m_population[0].fitness_function()));

        // Copy global generated object attributes into parents
        copy(object_attributes1.begin(), object_attributes1.end(),
            parents.first.obj_var_begin());

        copy(object_attributes2.begin(), object_attributes2.end(),
            parents.second.obj_var_begin());

        // Copy global generated step sizes into parents
        copy(step_sizes1.begin(), step_sizes1.end(),
            parents.first.step_size_begin());

        copy(step_sizes2.begin(), step_sizes2.end(),
            parents.second.step_size_begin());

        return parents;
    }

    // ========================================================================
}