#include <algorithm>
#include <cmath>
#include <functional>
#include <iterator>
#include <random>
#include <vector>

#include "common/random_number_engine.h"
#include "evolution-strategy/individual.h"

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
	const double Individual::MIN_STEP_SIZE = 1e-3;

    Individual::Individual(
		size_type num_obj_var,
        function<double(const Individual&)> fitness_fn)
        : m_fitness_fn(fitness_fn), m_valid_fitness(false),
        m_proportionality_constant(1 / sqrt(num_obj_var))
    {
        m_data.resize(num_obj_var + 1);
    }

	const function<double(const Individual&)>& Individual::fitness_function() const
	{
		return m_fitness_fn;
	}

    Individual& Individual::operator=(const Individual& other)
    {
        if (&other != this)
        {
            m_data.clear();
            copy(other.begin(), other.end(), back_inserter(m_data));
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
		normal_distribution<double> ndist;
        // Mutate step size
        double& step_size = *step_size_begin();
        step_size *= exp(m_proportionality_constant * ndist(mt_engine));
        step_size = max(MIN_STEP_SIZE, step_size );

        // Mutate object variables
        iterator begin = obj_var_begin(), end = obj_var_end();
        for (iterator it = begin; it != end; it++)
        {
            double& value = *it;
            // Mutate the value
            value += step_size * ndist(mt_engine);
            
            // Keep the values in the [0, 1] range
            while (value > 1) value -= 1;
            while (value < 0) value += 1;
        }
    }

    double Individual::fitness()
    {
        if (!m_valid_fitness)
        {
            m_fitness_val = m_fitness_fn(*this);
            m_valid_fitness = true;
        }

        return m_fitness_val;
    }
}