#include <iostream> // test

#include <algorithm>
#include <cmath>
#include <functional>
#include <iterator>
#include <random>
#include <vector>

#include "evolution-strategy/evolution_strategy.h"
#include "evolution-strategy/individual.h"
#include "random/random_number_engine.h"

using std::abs;
using std::back_inserter;
using std::copy;
using std::exp;
using std::function;
using std::max;
using std::min;
using std::mt19937;
using std::normal_distribution;
using std::sqrt;
using std::uniform_real_distribution;
using std::vector;

static const double COMP_EPSILON = 1e-7;

namespace es
{
	const double Individual::MIN_STEP_SIZE = 1e-7;
	const double Individual::MAX_STEP_SIZE = 0.1;

    Individual::Individual(
		size_type num_obj_var,
		Evolution_Strategy::fitness_function* fitness_fn)
        : m_fitness_fn(fitness_fn), m_valid_fitness(false)
    {
        m_data.resize(num_obj_var + 1);
		mt19937& mt_engine = random::mt_engine_singleton();
		
		// Initialize the object variables
		uniform_real_distribution<double> uniform_distribution(0.0, 1.0);
		for (Individual::iterator it = obj_var_begin(); it != obj_var_end(); ++it)
			*it= uniform_distribution(mt_engine);
		
		// Initialize the step size
		uniform_distribution = uniform_real_distribution<double>(MIN_STEP_SIZE, MAX_STEP_SIZE);
		*step_size_begin() = uniform_distribution(mt_engine);
    }

	Evolution_Strategy::fitness_function* Individual::fitness_function() const
	{
		return m_fitness_fn;
	}

    Individual& Individual::operator=(const Individual& other)
    {
        if (&other != this)
        {
            m_data.clear();
            copy(other.begin(), other.end(), back_inserter(m_data));
			m_fitness_fn = other.m_fitness_fn;
			m_fitness_val = other.m_fitness_val;
			m_valid_fitness = other.m_valid_fitness;
        }

        return *this;
    }

    bool Individual::operator==(const Individual& other) const
    {
        if (&other == this)
            return true;

        if (size() != other.size())
            return false;

        double accumulator = 0;
        for (size_type i = 0; i < size(); i++)
            accumulator += abs(m_data[i] - other[i]);

        return accumulator <= COMP_EPSILON;
    }

    void Individual::mutate()
    {
		mt19937& mt_engine = random::mt_engine_singleton();
		normal_distribution<double> step_size_normal_dist(0.0, 0.4);
		normal_distribution<double> obj_var_normal_dist;
        // Mutate step size
        double& step_size = *step_size_begin();
        step_size *= exp(step_size_normal_dist(mt_engine));
        step_size = min(MAX_STEP_SIZE, max(MIN_STEP_SIZE, step_size));

		// Mutate object variables
        iterator begin = obj_var_begin(), end = obj_var_end();
        for (iterator it = begin; it != end; it++)
        {
            double& value = *it;
            // Mutate the value
            value += step_size * obj_var_normal_dist(mt_engine);
            
            // Keep the values in the [0, 1] range
            while (value > 1) value -= 1;
            while (value < 0) value += 1;
        }
    }

    double Individual::fitness()
    {
        if (!m_valid_fitness)
        {
            m_fitness_val = (*m_fitness_fn)(*this);
			m_valid_fitness = true;
        }

        return m_fitness_val;
    }

	void Individual::expand(size_type amount)
	{
		if (amount == 0)
			return;

		vector<double> expansion_data;
		expansion_data.reserve(amount);
		
		mt19937& mt_engine = random::mt_engine_singleton();
		uniform_real_distribution<double> uniform_dist(0.0, 1.0);
		for (int i = 0; i < amount; ++i)
			expansion_data.push_back(uniform_dist(mt_engine));

		m_data.insert(obj_var_end(), expansion_data.begin(), expansion_data.end());
		m_valid_fitness = false;
	}
}