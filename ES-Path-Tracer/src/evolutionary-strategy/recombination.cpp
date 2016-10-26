#include <algorithm>
#include <random>
#include <stdexcept>
#include <vector>

#include "evolutionary-strategy/individual.h"
#include "evolutionary-strategy/random_engine.h"
#include "evolutionary-strategy/recombination.h"

using std::copy;
using std::logic_error;
using std::mt19937;
using std::uniform_int_distribution;
using std::uniform_real_distribution;
using std::vector;

namespace ES {
	static double alpha = 0.5;
	static bool random_alpha = false;

	// ========================================================================
	// =============================== UTILITY ================================
	// ========================================================================

	typedef vector<double>(*individual_recombination_function)(
		Individual::const_iterator begin1, Individual::const_iterator end1, 
		Individual::const_iterator begin2);

	vector<double> discrete_recombination( Individual::const_iterator begin1, 
		Individual::const_iterator end1, Individual::const_iterator begin2 )
	{
		uniform_int_distribution<int> coin(0, 1);
		vector<double> recomb_vec;

		while (begin1 != end1)
		{
			if ( coin(ES::mt_engine) )
				recomb_vec.push_back(*begin1);
			else
				recomb_vec.push_back(*begin2);

			begin1++;
			begin2++;
		}

		return recomb_vec;
	}

	vector<double> interpolation_recombination(Individual::const_iterator begin1,
		Individual::const_iterator end1, Individual::const_iterator begin2)
	{
		vector<double> recomb_vec;

		while (begin1 != end1)
		{
			double interp_val = alpha * (*begin1) + (1 - alpha) * (*begin2);
			recomb_vec.push_back( interp_val );

			begin1++;
			begin2++;
		}

		return recomb_vec;
	}

	Individual recombination(const Individual& parent1, const Individual& parent2,
		individual_recombination_function obj_var_recomb_fn, 
		individual_recombination_function std_dev_recomb_fn)
	{
		if (parent1.size() != parent2.size())
			throw logic_error("Parents of different size");

		/*	Perform recombination of the parents' object variables according to the 
			specified obj_var_recomb_fn function */
		vector<double> child_obj_var = obj_var_recomb_fn( parent1.obj_var_begin(),
			parent1.obj_var_end(), parent2.obj_var_begin() );

		/*	Perform recombination of the parents' standarrd deviations according to 
			the specified std_dev_recomb_fn function */
		vector<double> child_std_dev = std_dev_recomb_fn( parent1.std_dev_begin(),
			parent1.std_dev_end(), parent2.std_dev_begin() );

		Individual child(child_obj_var.size(), parent1.fitness_fn);

		// Copy object attributes
		copy(child_obj_var.begin(), child_obj_var.end(), child.obj_var_begin());
		// Copy standard deviations
		copy(child_std_dev.begin(), child_std_dev.end(), child.std_dev_begin());

		return child;
	}

	// ========================================================================

	

	// ========================================================================
	// ======================= RECOMBINATION FUNCTIONS ========================
	// ========================================================================

	Individual discrete_recombination(const Individual& parent1, 
		const Individual& parent2)
	{
		return recombination( parent1, parent2, discrete_recombination, 
			discrete_recombination );
	}

	Individual interpolation_recombination(const Individual& parent1,
		const Individual& parent2)
	{
		if (random_alpha)
		{
			uniform_real_distribution<double> dist(0, 1);
			alpha = dist(ES::mt_engine);
		}

		return recombination( parent1, parent2, interpolation_recombination, 
			interpolation_recombination );
	}

	Individual hibrid_recombination(const Individual& parent1,
		const Individual& parent2)
	{
		if (random_alpha)
		{
			uniform_real_distribution<double> dist(0, 1);
			alpha = dist(ES::mt_engine);
		}

		return recombination( parent1, parent2, discrete_recombination,
			interpolation_recombination );
	}

	// ========================================================================
}