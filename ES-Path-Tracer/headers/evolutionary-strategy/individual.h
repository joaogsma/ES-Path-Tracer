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
		iterator begin() { return data.begin(); }
		iterator end() { return data.end(); }
		const_iterator begin() const { return data.begin(); }
		const_iterator end() const { return data.end(); }

		// Iterator functions for the individual's object attributes
		iterator obj_attr_begin() { return data.begin(); }
		iterator obj_attr_end() { return std_dev_begin(); }
		const_iterator obj_attr_begin() const { return data.begin(); }
		const_iterator obj_attr_end() const { return std_dev_begin(); }

		// Iterator functions for the individual's standard deviation
		iterator std_dev_begin() { return --data.end(); }
		iterator std_dev_end() { return data.end(); }
		const_iterator std_dev_begin() const { return --data.end(); }
		const_iterator std_dev_end() const { return data.end(); }

		size_type size() const { return data.size(); }

		// Indexing operator
		double& operator[](size_type position) { return data[position]; }
		const double& operator[](size_type position) const { return data[position]; }

		Individual& operator=(const Individual& other);

		bool operator==(const Individual& other) const;

		void mutate();

		/*	Computes the fitness of the individual. The computation is performed
			only in the first call to this function, and then stored for O(1) access
			in subsequent calls */
		double fitness();

	private:
		bool valid_fitness;
		std::vector<double> data;
		double fitness_val;

		// Fitness function
		double(*fitness_fn)(const Individual& individual);
	};
}

#endif
