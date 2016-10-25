#ifndef __GUARD_INDIVIDUAL_H__
#define __GUARD_INDIVIDUAL_H__

#include <random>
#include <vector>


namespace ES
{
	/*	Individual in an Evolutionary Strategy algorithm. This implementation assumes that
		the individuals have only one sigma value, and mutate by applying this same value
		to a normal curve in each object attribute */
	class Individual {
	public:
		typedef std::vector<double>::size_type size_type;
		typedef std::vector<double>::iterator iterator;
		typedef std::vector<double>::const_iterator const_iterator;

		static const double COMP_EPSILON;

		Individual(int num_obj_attr, double(*fitness_fn)(const Individual& individual));
		Individual(int num_obj_attr, std::mt19937 random_engine,
			double(*fitness_fn)(const Individual& individual));

		// Iterator functions
		iterator begin() { return attributes.begin(); }
		iterator end() { return attributes.end(); }
		const_iterator begin() const { return attributes.begin(); }
		const_iterator end() const { return attributes.end(); }

		size_type size() const { return attributes.size(); }

		// Indexing operator
		double& operator[](size_type position) { return attributes[position]; }
		const double& operator[](size_type position) const { return attributes[position]; }

		Individual& operator=(const Individual& other);

		bool operator==(const Individual& other) const;

		void mutate();

		double fitness();

	private:
		bool valid_fitness;
		std::vector<double> attributes;
		double fitness_val;

		// Fitness function
		double(*fitness_fn)(const Individual& individual);
	};
}

#endif
