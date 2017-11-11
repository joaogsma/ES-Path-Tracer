#include <algorithm>
#include <random>
#include <stdexcept>
#include <vector>

#include "evolution-strategy/individual.h"
#include "evolution-strategy/recombination.h"
#include "random/random_number_engine.h"

using std::copy;
using std::logic_error;
using std::mt19937;
using std::uniform_int_distribution;
using std::uniform_real_distribution;
using std::vector;

using es::Individual;

namespace es {
	double Recombination::alpha = 0.5;
	bool Recombination::random_alpha = false;

	Recombination::Recombination()
	{
		throw std::runtime_error("Class must not be instantiated");
	}

	// ========================================================================
    // =============================== UTILITY ================================
    // ========================================================================

    vector<double> Recombination::discrete_recombination(
		Individual::const_iterator begin1,
        Individual::const_iterator end1,
		Individual::const_iterator begin2)
    {
		mt19937& mt_engine = random::mt_engine_singleton();
		uniform_int_distribution<int> coin(0, 1);
        vector<double> recomb_vec;
		
        while (begin1 != end1)
        {
            
			if (coin(mt_engine))
                recomb_vec.push_back(*begin1);
            else
                recomb_vec.push_back(*begin2);

            begin1++;
            begin2++;
        }

        return recomb_vec;
    }

    vector<double> Recombination::discrete_block_recombination(
		Individual::const_iterator begin1,
        Individual::const_iterator end1,
		Individual::const_iterator begin2)
    {
		mt19937& mt_engine = random::mt_engine_singleton();
        uniform_int_distribution<int> coin(0, 1);
        vector<double> recomb_vec;

        while (begin1 != end1)
        {
            Individual::const_iterator chosen;

            if (coin(mt_engine))
                chosen = begin1;
            else
                chosen = begin2;

            /*  Add the chosen value (chosen[0]) and the corresponding next two
                values (chosen[1] and chosen[2]). These correspond to the
                normalize of a diffuse reflected ray, if the type of secondary
                ray specified by chosen[0] is diffuse reflection */
            recomb_vec.push_back(chosen[0]);
            recomb_vec.push_back(chosen[1]);
            recomb_vec.push_back(chosen[2]);

            begin1 += 3;
            begin2 += 3;
        }

        return recomb_vec;
    }

    vector<double> Recombination::interpolation_recombination(
		Individual::const_iterator begin1,
        Individual::const_iterator end1,
		Individual::const_iterator begin2)
    {
        vector<double> recomb_vec;

        while (begin1 != end1)
        {
            double interp_val = Recombination::alpha * (*begin1)
				+ (1 - Recombination::alpha) * (*begin2);
            recomb_vec.push_back(interp_val);

            begin1++;
            begin2++;
        }

        return recomb_vec;
    }


    Individual Recombination::recombination(
		const Individual& parent1,
		const Individual& parent2,
        individual_recombination_function obj_var_recomb_fn,
        individual_recombination_function step_size_recomb_fn)
    {
        if (parent1.size() != parent2.size())
            throw logic_error("Parents of different size");

        /*	Perform recombination of the parents' object variables according to the
            specified obj_var_recomb_fn function */
        vector<double> child_obj_var = obj_var_recomb_fn(parent1.obj_var_begin(),
            parent1.obj_var_end(), parent2.obj_var_begin());

        /*	Perform recombination of the parents' standard deviations according to
            the specified step_size_recomb_fn function */
        vector<double> child_step_size = step_size_recomb_fn(parent1.step_size_begin(),
            parent1.step_size_end(), parent2.step_size_begin());

        Individual child(child_obj_var.size(), parent1.fitness_function());

        // Copy object attributes
        copy(child_obj_var.begin(), child_obj_var.end(), child.obj_var_begin());
        // Copy step sizes
        copy(child_step_size.begin(), child_step_size.end(), child.step_size_begin());

        return child;
    }

    // ========================================================================



    // ========================================================================
    // ======================= RECOMBINATION FUNCTIONS ========================
    // ========================================================================

    Individual Recombination::discrete_recombination(
		const Individual& parent1,
		const Individual& parent2)
    {
        return recombination(parent1, parent2, discrete_recombination,
            discrete_recombination);
    }

    Individual Recombination::interpolation_recombination(
		const Individual& parent1,
        const Individual& parent2)
    {
        // Remember current alpha value
        double old_alpha = Recombination::alpha;

        if (Recombination::random_alpha)
        {
			mt19937& mt_engine = random::mt_engine_singleton();
			uniform_real_distribution<double> dist(0, 1);
            Recombination::alpha = dist(mt_engine);
        }

        Individual& child = recombination(parent1, parent2, interpolation_recombination,
            interpolation_recombination);

        // Restore previous alpha value
        Recombination::alpha = old_alpha;

        return child;
    }

    Individual Recombination::hibrid_recombination(const Individual& parent1,
        const Individual& parent2)
    {
        // Remember current alpha value
        double old_alpha = Recombination::alpha;

        if (Recombination::random_alpha)
        {
			mt19937& mt_engine = random::mt_engine_singleton();
            uniform_real_distribution<double> dist(0, 1);
			Recombination::alpha = dist(mt_engine);
        }

        Individual& child = recombination(parent1, parent2, discrete_recombination,
            interpolation_recombination);

        // Restore previous alpha value
        Recombination::alpha = old_alpha;

        return child;
    }

    Individual Recombination::discrete_path_tracer_block_recombination(const Individual& parent1,
        const Individual& parent2)
    {
        return recombination(
			parent1,
			parent2,
			discrete_block_recombination,
			discrete_recombination
		);
    }

    // ========================================================================
}