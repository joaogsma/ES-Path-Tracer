#include <algorithm>
#include <vector>

#include "evolution-strategy/survivor_selection.h"

using std::vector;
using std::sort;
using es::Individual;

namespace es
{
	Survivor_Selection::Survivor_Selection()
	{
		throw std::runtime_error("Class must not be instantiated");
	}

    bool Survivor_Selection::compare(Individual& indiv1, Individual& indiv2)
    {
        return indiv1.fitness() > indiv2.fitness();
    }

    void Survivor_Selection::select_survivors(
		vector<Individual>& individual_vec,
        vector<Individual>::size_type num_survivors)
    {
        sort(individual_vec.begin(), individual_vec.end(), compare);
        individual_vec.erase( individual_vec.begin() + num_survivors, 
            individual_vec.end() );
    }

    void Survivor_Selection::generational_selection(
		vector<Individual>& m_population,
        vector<Individual> children)
    {
        select_survivors(children, m_population.size());
        m_population = children;
    }

    void Survivor_Selection::nongenerational_selection(
		vector<Individual>& m_population,
        vector<Individual> children)
    {
        children.insert( m_population.end(), children.begin(), children.end() );
        select_survivors(children, m_population.size());
        m_population = children;
    }
}