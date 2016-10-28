#include <algorithm>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "evolutionary-strategy/individual.h"
#include "evolutionary-strategy/parent_selection.h"
#include "evolutionary-strategy/random_engine.h"

using std::copy;
using std::logic_error;
using std::make_pair;
using std::mt19937;
using std::pair;
using std::uniform_int_distribution;
using std::vector;

namespace ES
{
    namespace parent_selection
    {
        // ========================================================================
        // =============================== UTILITY ================================
        // ========================================================================

        typedef Individual::const_iterator(Individual::*const_iterator_function)() const;

        /*	This function fills the specified vectors with values globally uniformly
            selected (using the specified random engine) from the given population.
            The values from the population individuals, as well as the vector sizes,
            are extracted from the intervals specified by the iterator functions */
        void global_fill_vector(vector<double>& parent1_vec, vector<double>& parent2_vec,
            const vector<Individual>& population, const_iterator_function begin,
            const_iterator_function end)
        {
            parent1_vec.clear();
            parent2_vec.clear();

            vector<double>::size_type size = (population[0].*begin)() - (population[0].*end)();

            // Uniform distribution for the population vector
            uniform_int_distribution<vector<Individual>::size_type> indiv_dist(0,
                population.size() - 1);

            for (vector<double>::size_type i = 0; i < size; i++)
            {
                vector<Individual>::size_type parent1_index, parent2_index;
                // Select a first parent
                parent1_index = indiv_dist(ES::mt_engine);
                // Select a second parent different from the first
                while ((parent2_index = indiv_dist(ES::mt_engine)) == parent1_index);

                // Get the selected parents
                const Individual& parent1 = population[parent1_index];
                const Individual& parent2 = population[parent2_index];

                // Get the values from the parents
                double value_parent1 = (parent1.*begin)()[i];
                double value_parent2 = (parent2.*begin)()[i];

                // Store the values
                parent1_vec[i] = value_parent1;
                parent2_vec[i] = value_parent2;
            }
        }

        // ========================================================================



        // ========================================================================
        // ====================== PARENT SELECTION FUNCTIONS ======================
        // ========================================================================

        /*	Local uniform selection. This function selects two different parents
            with uniform probability from amongst the population and returns them */
        std::pair<Individual, Individual> local_uniform_selection(
            const vector<Individual>& population)
        {
            if (population.empty())
                throw logic_error("Empty population");

            // Uniform distribution for the population vector
            uniform_int_distribution<vector<Individual>::size_type> indiv_dist(0,
                population.size() - 1);

            // Select a first parent
            vector<Individual>::size_type parent1_index = indiv_dist(ES::mt_engine);
            // Select a second parent different from the first
            vector<Individual>::size_type parent2_index;
            while ((parent2_index = indiv_dist(ES::mt_engine)) == parent1_index);

            return make_pair(population[parent1_index], population[parent2_index]);
        }

        /*	Global uniform selection. This function selects each object attribute
            and each standard deviation from a uniformly selected individual from
            the population, sampled independently for each value. Corresponding
            attributes in both generated parents are guaranteed to come from
            different individuals in the population */
        std::pair<Individual, Individual> global_uniform_selection(
            const vector<Individual>& population)
        {
            if (population.empty())
                throw logic_error("Empty population");

            // Vectors for the generated parents's data
            vector<double> object_attributes1, object_attributes2, standard_deviations1,
                standard_deviations2;

            // Fill the object attributes vectors
            global_fill_vector(object_attributes1, object_attributes2, population,
                &Individual::obj_var_begin, &Individual::obj_var_end);

            // Fill the standard deviations vectors
            global_fill_vector(standard_deviations1, standard_deviations2, population,
                &Individual::std_dev_begin, &Individual::std_dev_end);

            int num_obj_var = (int)object_attributes1.size();

            // Create return parents
            pair<Individual, Individual> parents = make_pair(
                Individual(num_obj_var, population[0].fitness_fn),
                Individual(num_obj_var, population[0].fitness_fn));

            // Copy global generated object attributes into parents
            copy(object_attributes1.begin(), object_attributes1.end(),
                parents.first.obj_var_begin());

            copy(object_attributes2.begin(), object_attributes2.end(),
                parents.second.obj_var_begin());

            // Copy global generated standard deviations into parents
            copy(standard_deviations1.begin(), standard_deviations1.end(),
                parents.first.std_dev_begin());

            copy(standard_deviations2.begin(), standard_deviations2.end(),
                parents.second.std_dev_begin());

            return parents;
        }

        // ========================================================================
    }
}