#include <algorithm>
#include <cmath>
#include <iterator>
#include <random>
#include <vector>

#include "evolutionary-strategy/individual.h"

using std::abs;
using std::back_inserter;
using std::copy;
using std::mt19937;
using std::uniform_real_distribution;
using std::vector;

namespace ES
{
	// Epsilon constant used in Individual comparations
	const double Individual::COMP_EPSILON = 10E-9;

	Individual::Individual(int num_obj_attr,
		double(*fitness_fn)(const Individual& individual))
		: fitness_fn(fitness_fn), valid_fitness(false)
	{
		attributes.resize(num_obj_attr + 1);
	}

	Individual::Individual(int num_obj_attr, std::mt19937 random_engine,
		double(*fitness_fn)(const Individual& individual))
		: fitness_fn(fitness_fn), valid_fitness(false)
	{
		uniform_real_distribution<double> dist(0, 1);

		for (int i = 0; i < num_obj_attr + 1; i++)
			attributes.push_back(dist(random_engine));
	}

	Individual& Individual::operator=(const Individual& other)
	{
		if (&other != this)
		{
			attributes.clear();
			copy(other.begin(), other.end(), back_inserter(attributes));
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
			accumulator += abs(attributes[i] - other[i]);

		return accumulator <= COMP_EPSILON;
	}

	void Individual::mutate()
	{

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