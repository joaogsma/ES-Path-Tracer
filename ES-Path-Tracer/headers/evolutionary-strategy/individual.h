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
		typedef std::vector<double>::value_type value_type;

		static const double COMP_EPSILON;

		Individual(size_type num_obj_var, double(*fitness_fn)(const Individual& individual));
		Individual(size_type num_obj_var, std::mt19937 random_engine,
			double(*fitness_fn)(const Individual& individual));
		
		// Fitness function pointer
		double(*fitness_fn)(const Individual& individual);

		// Iterator functions
		iterator begin() { return data.begin(); }
		iterator end() { return data.end(); }
		const_iterator begin() const { return data.begin(); }
		const_iterator end() const { return data.end(); }

		// Iterator functions for the individual's object attributes
		iterator obj_var_begin() { return data.begin(); }
		iterator obj_var_end() { return std_dev_begin(); }
		const_iterator obj_var_begin() const { return data.begin(); }
		const_iterator obj_var_end() const { return std_dev_begin(); }

		// Iterator functions for the individual's standard deviation
		iterator std_dev_begin() { return --data.end(); }
		iterator std_dev_end() { return data.end(); }
		const_iterator std_dev_begin() const { return --data.end(); }
		const_iterator std_dev_end() const { return data.end(); }

		// Size functions
		size_type size() const { return data.size(); }
		size_type obj_var_size() const { return obj_var_end() - obj_var_begin(); }
		size_type std_dev_size() const { return std_dev_end() - std_dev_begin(); }

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
	};
}

#endif
