#include <algorithm>
#include <vector>

#include "evolution-strategy/survivor_selection.h"

using std::vector;
using std::sort;

using es::individual::Individual;

namespace es
{
    namespace survivor_selection
    {
        bool indiv_compare(Individual& indiv1, Individual& indiv2)
        {
            return indiv1.fitness() > indiv2.fitness();
        }

        void select_survivors(vector<Individual>& individual_vec, 
            vector<Individual>::size_type num_survivors)
        {
            sort(individual_vec.begin(), individual_vec.end(), indiv_compare);
            individual_vec.erase( individual_vec.begin() + num_survivors, 
                individual_vec.end() );
        }

        void generational_selection(vector<Individual>& population,
            vector<Individual> children)
        {
            select_survivors(children, population.size());
            population = children;
        }

        void nongenerational_selection(vector<Individual>& population,
            vector<Individual> children)
        {
            children.insert( population.end(), children.begin(), children.end() );
            select_survivors(children, population.size());
            population = children;
        }
    }
}