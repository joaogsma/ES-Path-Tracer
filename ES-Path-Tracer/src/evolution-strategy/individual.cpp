#include <algorithm>
#include <cmath>
#include <iterator>
#include <random>
#include <vector>

#include "evolution-strategy/individual.h"
#include "evolution-strategy/random_engine.h"

using std::abs;
using std::back_inserter;
using std::copy;
using std::exp;
using std::max;
using std::min;
using std::mt19937;
using std::normal_distribution;
using std::sqrt;
using std::uniform_real_distribution;
using std::vector;

namespace es
{
    namespace individual
    {
        Individual::Individual(size_type num_obj_var,
            double(*fitness_fn)(const Individual& individual))
            : fitness_fn(fitness_fn), valid_fitness(false),
            proportionality_constant(1 / sqrt(num_obj_var))
        {
            data.resize(num_obj_var + 1);
        }

        Individual::Individual(size_type num_obj_var, std::mt19937 random_engine,
            double(*fitness_fn)(const Individual& individual))
            : fitness_fn(fitness_fn), valid_fitness(false),
            proportionality_constant(1 / sqrt(num_obj_var))
        {
            uniform_real_distribution<double> dist(0, 1);

            for (size_type i = 0; i < num_obj_var + 1; i++)
                data.push_back(dist(random_engine));
        }

        Individual& Individual::operator=(const Individual& other)
        {
            if (&other != this)
            {
                data.clear();
                copy(other.begin(), other.end(), back_inserter(data));
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
                accumulator += abs(data[i] - other[i]);

            return accumulator <= COMP_EPSILON;
        }

        void Individual::mutate()
        {
            normal_distribution<double> ndist;
            // Mutate step size
            double& step_size = *step_size_begin();
            step_size *= exp( proportionality_constant * ndist(es::m_mt_engine) );
            step_size = max( MIN_STEP_SIZE, step_size );

            // Mutate object variables
            iterator begin = obj_var_begin(), end = obj_var_end();
            for (iterator it = begin; it != end; it++)
            {
                double& value = *it;
                // Mutate the value
                value += step_size * ndist(es::m_mt_engine);
                
                // Keep the values in the [0, 1] range
                while (value > 1) value -= 1;
                while (value < 0) value += 1;
            }
        }

        double Individual::fitness()
        {
            if (!valid_fitness)
            {
                fitness_val = fitness_fn(*this);
                valid_fitness = true;
            }

            return fitness_val;
        }
    }
}