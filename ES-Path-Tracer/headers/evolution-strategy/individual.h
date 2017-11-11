#ifndef __GUARD_INDIVIDUAL_H__
#define __GUARD_INDIVIDUAL_H__

#include <functional>
#include <random>
#include <vector>

#include "evolution_strategy.h"

namespace es
{
    /*	Individual in an evolution Strategy algorithm. This implementation assumes that
        the individuals have only one sigma value, and mutate by applying this same value
        to a normal curve in each object attribute */
    class Individual {
		static const double MIN_STEP_SIZE;
	
	public:
        typedef std::vector<double>::size_type size_type;
        typedef std::vector<double>::iterator iterator;
        typedef std::vector<double>::const_iterator const_iterator;
        typedef std::vector<double>::value_type value_type;

        Individual(
			size_type num_obj_var,
			Evolution_Strategy::fitness_function* fitness_fn);

		Evolution_Strategy::fitness_function* fitness_function() const;

		void expand(size_type amount);
        
			// Iterator functions
        iterator begin() { return m_data.begin(); }
        iterator end() { return m_data.end(); }
        const_iterator begin() const { return m_data.begin(); }
        const_iterator end() const { return m_data.end(); }

        // Iterator functions for the individual's object attributes
        iterator obj_var_begin() { return m_data.begin(); }
        iterator obj_var_end() { return step_size_begin(); }
        const_iterator obj_var_begin() const { return m_data.begin(); }
        const_iterator obj_var_end() const { return step_size_begin(); }

        // Iterator functions for the individual's step sizes
        iterator step_size_begin() { return --m_data.end(); }
        iterator step_size_end() { return m_data.end(); }
        const_iterator step_size_begin() const { return --m_data.end(); }
        const_iterator step_size_end() const { return m_data.end(); }

        // Size functions
        size_type size() const { return m_data.size(); }
        size_type obj_var_size() const { return obj_var_end() - obj_var_begin(); }
        size_type step_size_size() const { return step_size_end() - step_size_begin(); }

        // Indexing operator
        double& operator[](size_type position) { return m_data[position]; }
        const double& operator[](size_type position) const { return m_data[position]; }

        Individual& operator=(const Individual& other);

        bool operator==(const Individual& other) const;

        void mutate();

        /*	Computes the fitness of the individual. The computation is performed
            only in the first call to this function, and then stored for O(1) access
            in subsequent calls */
        double fitness();

    private:
		Evolution_Strategy::fitness_function* m_fitness_fn;
        std::vector<double> m_data;
        double m_proportionality_constant;
        double m_fitness_val;
        bool m_valid_fitness;
    };
}

#endif
