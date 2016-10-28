#ifndef __GUARD_FITNESS_H__
#define __GUARD_FITNESS_H__

#include "../path-tracer/path_tracer.h"

#include "individual.h"
#include "pixel_histogram.h"

namespace ES
{
    namespace fitness
    {
        static Pixel_Histogram histogram;

        double pixel_histogram_fitness_fn(const Individual& individual);
    }
}

#endif