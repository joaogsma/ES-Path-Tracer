#ifndef __GUARD_RECOMBINATION_H
#define __GUARD_RECOMBINATION_H

#include "individual.h"
#include "random_engine.h"

namespace es {
    namespace recombination
    {
        static double alpha = 0.5;
        static bool random_alpha = false;

        /*  Discrete recombination function. The value for each position in the 
            child (object variables and step sizes) is randomly chosen 
            from amongst the values in the parents' corresponding positions. The 
            choice is made independently for each position */
        individual::Individual discrete_recombination(
            const individual::Individual& parent1,
            const individual::Individual& parent2);

        /*  Interpolation recombination function. The child is the interpolation of
            both parent vectors, with weights alpha and (1 - alpha) for parent1 and 
            parent2, respectively. Default alpha value is 0.5 and, if random_alpha is
            set to true, alpha is randomly chosen from the range [0, 1] for each child */
        individual::Individual interpolation_recombination(
            const individual::Individual& parent1,
            const individual::Individual& parent2);

        /*  Hibrid recombination function. The object variables in the child are 
            recombined as in the discrete recombination function, and the standard
            deviation values are the interpolation of the parents's, as in the 
            interpolation recombination function */
        individual::Individual hibrid_recombination(
            const individual::Individual& parent1,
            const individual::Individual& parent2);

        /*  Discrete path tracer block recombination. This heuristic is an adaptation
            of the discrete recombination that tries to take advantage of the fact that
            in the context of a Path Tracer, an ES Individual is a sequence of secondary
            ray decisions, and each decision corresponds to 3 consecutive positions in
            the Individual vector. The recombination is made similarly to the standard
            discrete recombination, but values are copied from the parents in blocks of 3,
            instead of independently for every position */
        individual::Individual discrete_path_tracer_block_recombination(
            const individual::Individual& parent1, 
            const individual::Individual& parent2);
    }
}

#endif
